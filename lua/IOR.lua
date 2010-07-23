
local math     = math
local string   = string
local tonumber = tonumber

module "IOR"

local cursor = 1
local _ior   = nil

function hexa2byte(value)
  value = string.gsub(
    value, 
    "(%x%x)", 
    function(hexa)
      return string.char(tonumber(hexa, 16))
    end
    )
  return value
end  

function align(pattern)
  local padding = math.mod(cursor - 1, pattern)
  if (padding > 0) then
    cursor = cursor + (pattern - padding)
  end
end

function getUShort()
  align(2)
  local ushort
  local b1 = _ior:byte(cursor)
  local b2 = _ior:byte(cursor + 1)
  if (order == "bigEndian") then
    ushort = b2 + (256*b1)
  else
    ushort = b1 + (256*b2)
  end
  cursor = cursor + 2
  return ushort
end

function getULong()
  align(4)
  local ulong
  local b1 = _ior:byte(cursor)
  local b2 = _ior:byte(cursor + 1)
  local b3 = _ior:byte(cursor + 2)
  local b4 = _ior:byte(cursor + 3)
  if (order == "bigEndian") then
    ulong = b4 + (256*b3) + (65536*b2) + (16777216*b1)
  else
    ulong = b1 + (16*b2) + (65536*b3) + (16777216*b4)
  end
  cursor = cursor + 4
  return ulong
end

function getString()
  local length = getULong()
  local str = _ior:sub(cursor, (cursor + length) - 1)
  cursor = cursor + length
  return str
end

function IIOPProfileGetObjectKey(ior)
  _ior = string.match(ior, "^IOR:(.+)$")
  _ior = hexa2byte(_ior)
  cursor = 1
  
-- Byte order of IOR.
  local order = _ior:byte()
  cursor = cursor + 1
  if order then
    order = "littleEndian"
  else
    order = "bigEndian"
  end

-- TypeId
  typeId = getString() 
-- numProfiles
  numProfiles = getULong()

-- IIOP Profile
-- tag
  IIOPTag = getULong()
-- length
  IIOPProfileLength = getULong()
-- Byte order
  IIOPProfileBOrder = _ior:byte(cursor)
  cursor = cursor + 1
-- Major version
  IIOPProfileMajor = _ior:byte(cursor)
  cursor = cursor + 1
-- Minor version
  IIOPProfileMinor = _ior:byte(cursor)
  cursor = cursor + 1
-- Host 
  IIOPProfilerHost = getString()
-- Port
  IIOPProfilerPort = getUShort()
-- Object key
  objectKey = getString()
  return objectKey
end
