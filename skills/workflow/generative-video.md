# Generative Video Rules

Rules for projects that generate procedural, parameter-driven animation and render it to video files: abstract visuals for social media, music videos, projection content. They do not cover editing existing video, AI video-generation models or live VJ performance. This skill sits on top of a platform skill such as `platform/python-cli` or `platform/python-mac`, which owns the environment, dependency, packaging and build rules.

---

**Look at the reference images before designing anything**
Before designing a new look, read the images at the link in `.figma-references-url` through the Figma MCP, save them to a gitignored `References/` folder, and write down what they have in common before writing any shader. If the link is missing or Figma isn't connected, ask for it. Don't design from guesses.

**A failed response looks like:**
- Designing a look without looking at the reference images
- Carrying on without the references when the Figma connection fails
- Committing the downloaded reference images

---

**Pixels on the GPU, control in the host language**
Frames are drawn with shaders (e.g. GLSL through moderngl in Python), not per-pixel loops. The host language handles parameters, randomness, automation and encoding, and passes values to the shaders. Keep the two apart: rendering code draws a frame from the values it is given, and control code decides what those values are.

**A failed response looks like:**
- Looping over pixels in Python to draw a frame
- Using numpy image maths for frames when it cannot reach the target resolution in reasonable time
- Rendering logic tangled into the code that handles parameters, automation or encoding

---

**Render through an off-screen GPU context, with ffmpeg checked up front**
Renders run offline through an off-screen (standalone) GPU context, e.g. `moderngl.create_standalone_context()`, so a render needs no window. Install ffmpeg with Homebrew (`brew install ffmpeg`) and check it is on the PATH (e.g. `shutil.which("ffmpeg")`) before the first frame is drawn, not when encoding starts. Keep shaders as separate `.glsl` files. Shared code (noise, colour ramps, shading) lives in its own files, which the scene shaders include through an include step in the host before compiling, since GLSL has no include of its own.

**A failed response looks like:**
- A render that only works with a visible window open
- Failing on a missing ffmpeg after frames have already been rendered
- Shader source pasted into Python strings
- The same noise function copied into every shader instead of included from one shared file

---

**A seed plus the parameter set fully determines every frame**
Time comes from the frame index divided by the fps, never from the wall clock. All randomness flows from seeded generators in the host or seeded noise in the shader. Rendering the same settings twice must produce the same frames.

**A failed response looks like:**
- Calling an unseeded `random()` anywhere in the render
- `time.time()` or any other clock in a render path
- Two renders of the same settings producing different frames

---

**Every render writes a sidecar**
Next to each video, write a JSON file with the same name holding the seed, every parameter, the automation, the format, the fps and the tool version. That is enough to reproduce the video exactly, or to re-render any time range of it at another size.

**A failed response looks like:**
- A video with no record of how it was made
- Settings that only exist in terminal history

---

**Parameters move by automation**
Every parameter has a declared range and can be automated over time: smooth random drift, LFO, ramp, random jumps, keyframes or hold. A global energy curve scales how fast and how far everything moves. Automation is worked out in the host and passed to the shaders as uniforms every frame. Long renders keep varying throughout, so they can be cut up later in an editor.

**A failed response looks like:**
- Parameters that stay fixed for the whole render, with only the seed varying
- A value moving outside its declared range
- Automation hard-coded in a shader instead of passed in as uniforms

---

**Preview and render share one code path**
A preview window with sliders is for finding a look. The render path is offline with a fixed timestep. Both draw frames through the same code, so a render matches the preview at the same settings and time.

**A failed response looks like:**
- A render that looks different from the preview at the same settings and time
- Tuning a look by editing constants and re-running full renders instead of using the preview

---

**Encode for where the video is going**
Pipe raw frames straight into ffmpeg; never deliver a folder of PNGs. Defaults: H.264 MP4, `yuv420p`, `-movflags +faststart`, a high bitrate set explicitly, 30 or 60 fps. Offer ProRes on request for editing. Name formats by aspect ratio: `9x16` (1080x1920), `1x1` (1080x1080), `4x5` (1080x1350) and `16x9` (1920x1080), plus a 2x version of each for 4K (e.g. 2160x3840 for `9x16`). The format is chosen per render. On request, keep key content out of the platform UI safe zones on `9x16`.

**A failed response looks like:**
- Odd pixel dimensions, which `yuv420p` cannot encode
- An MP4 without `+faststart`
- Leaving the bitrate at a low default
- No way to choose the format for a render
- Delivering a folder of PNG frames instead of a video

---

**Renders go in a gitignored Output/ folder**
Create `Output/` at the project root and add it to `.gitignore` in the same commit. Video files are never committed. Presets and automation JSON are source, and are committed.

**A failed response looks like:**
- A video file committed to the repo
- Renders scattered across ad-hoc folders instead of `Output/`
- Leaving presets or automation JSON uncommitted because they sit next to renders

---

**Variations, not guesses**
To explore a look, batch-render many seeds or mutations of a preset into one flat folder, plus a contact sheet: a grid of frames, each labelled with its seed and timecode, so the Designer picks by eye. This is the video version of the parameter sweep rule in `workflow/dsp-prototyping`.

**A failed response looks like:**
- Rendering one variation and asking whether it looks right
- Splitting variations into a folder per parameter instead of one flat folder
- A batch of variations with no contact sheet

---

**Edit-friendly output**
Long renders write a cut list (JSON plus CSV or EDL) with the timecodes of scene changes, parameter jumps and energy peaks. Any time range of a past render can be re-rendered frame-exact from its sidecar, at any format.

**A failed response looks like:**
- No way to find the good moments in a long render without scrubbing through it
- Re-rendering a whole clip to get five seconds of it

---

**Show a frame, measure the cost**
After each step, render a sample and look at a frame before moving on. Whenever a shader or scene changes, report render speed in ms per frame at the target resolution. Tests cover determinism (identical frame hashes for identical settings), every scene at every format, and the loop seam when looping is used.

**A failed response looks like:**
- Claiming a visual works without rendering it and looking at a frame
- Reporting a shader or scene change with no timing figures
- A shader change that halves render speed without anyone noticing
