# lyxml

XML decoder for Lua based on [yxml](https://dev.yorhel.nl/yxml).

## Documentation

yxml has been modified to dynamically change its stack size when needed.

### Api

```lua
--- @param s string XML input
--- @returns table | (nil, string)
function lyxml.decode(s)
```

The returned table contains a tree of the elements. A table will have the element tag at index 0, the child nodes in index 1,2,3,... and the attributes stored with their respective keys as index.

### Example

```lua
local lyxml = require"lyxml"
local root, err = lyxml.decode[[
<this is="a">XML<Document /></this>
]]
-- root = { [0] = "this", is = "a", "XML", { [0] = "Document" } }
```

## License

MIT
