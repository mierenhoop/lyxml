# lyxml

XML decoder for Lua based on [yxml](https://dev.yorhel.nl/yxml).

## Documentation

yxml has been modified to dynamically change its stack size when needed.

### Api

### Decode

```lua
--- @param s string XML input
--- @param opt? { trimcontent: boolean|nil, tagfield: string|nil } options table
--- trimcontent which is true by default trims spaces/newlines around content strings
--- tagfield is the name of the field in which xml element names are stored
--- @returns table | (nil, string)
function lyxml.decode(s, opt) end
```

The returned table contains a tree of the elements. A table will have the element tag at index 0, the child nodes in index 1,2,3,... and the attributes stored with their respective keys as index.

### Encode

While there is a general purpose XML-encoder in [`encode.lua`](./encode.lua), it has some limitations:

1. Attribute order is not kept.

2. No indentation/pretty printing emitted.

3. No CDATA or comments emitted.

4. Elements are always self-closing when possible.

5. HTML-style standalone tags are not supported (also in the decoder).

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
