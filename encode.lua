local escmap = {
  ["&"] = "&amp;",
  ["<"] = "&lt;",
  [">"] = "&gt;",
  ["'"] = "&apos;",
  ['"'] = "&quot;",
}

local esccontent = "[&<>]"
local escattr = "[&'\"]"

local function escape(pat, s)
  return (string.gsub(s, pat, escmap))
end

local function decode(elem, opts, buf)
  if type(elem) == "table" then
    local tag = elem[opts.tagfield]
    buf[#buf+1] = "<"..tag
    for k, v in pairs(elem) do
      if type(k) == "string" then
        buf[#buf+1] = " "..k..'="'..escape(escattr, tostring(v))..'"'
      end
    end
    if #elem > 0 then
      buf[#buf+1] = ">"
      for i = 1, #elem do
        decode(elem[i], opts, buf)
      end
      buf[#buf+1] = "</"..tag..">"
    else
      buf[#buf+1] = " />"
    end
  else
    buf[#buf+1] = escape(esccontent, tostring(elem))
  end
end

return function(root, opts)
  opts = opts or {}
  opts.tagfield = opts.tagfield or 0
  buf = {}
  decode(root, opts, buf)
  return table.concat(buf)
end
