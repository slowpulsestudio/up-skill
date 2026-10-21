# Deep-Linking Rules

Rules for making hierarchical tree navigation fully URL-addressable, so any node can be linked to directly and the tree opens, expands and scrolls to it correctly on load.

---

**The route is the single source of truth for selection**
Derive the selected node ID from the current route, every render. Never keep a separate `selectedId` or `selectedNode` in local state. Sharing a URL, refreshing and navigating back then all land on the right node with no extra code. A click on a node only navigates; the selection follows from the new route. A root route with no node in it maps to a fixed default node (e.g. the first one).

**A failed response looks like:**
- Holding the selection in `useState` and updating the URL separately, so the two can drift apart
- A click handler that sets local selection state as well as navigating
- A deep link that loads the page but shows nothing selected

---

**Expand every ancestor of the selected node**
Whenever the selected ID changes, including on first render, walk up from the selected node to the root and add every ancestor to the expanded set. Merge them into what is already expanded rather than replacing it. A deep-linked node must always be visible, never hidden inside a collapsed branch.

**A failed response looks like:**
- Expanding branches only on user clicks, so a node reached by deep link stays hidden
- Replacing the expanded set on navigation and collapsing branches the user had opened

---

**Scroll the selected node into view**
The tree row scrolls itself into view whenever it becomes selected (e.g. an effect on `isSelected` calling `scrollIntoView({ block: 'nearest' })`). This covers a deep link to a node that starts off-screen.

**A failed response looks like:**
- Scrolling only inside a click handler, so nothing scrolls on deep-link arrival
- A deep link to a node far down the tree that leaves it off-screen

---

**Node IDs are stable and deterministic**
Node IDs live in the URL, so they must stay the same across page loads, reloads and navigations. Use IDs from the data source (a database primary key or a fixed slug), or generate them deterministically from fixed data such as the node name. Never derive them from array index, render order or random values.

**A failed response looks like:**
- Using an array index as the ID, so a link points at a different node once the data order changes
- Generating IDs with `crypto.randomUUID()` or similar on load, so every link breaks on reload
- Sharing a link that lands on the wrong node, or no node, after a data change

---

**Saved references store the full route**
Any feature that stores a node for later (favourites, recents, bookmarks) saves the full route (e.g. `href: '#/tree/users-ou'`) alongside the ID, at the time it is saved. The saved list then renders as plain links, with no need to load and walk the tree to rebuild each URL.

**A failed response looks like:**
- Storing only the node ID, so the saved list has to load the full tree before it can show a link
- A saved list that breaks or shows nothing when the tree data hasn't loaded yet
- Rebuilding saved links from a hard-coded route pattern that breaks when the route shape changes
