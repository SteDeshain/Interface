module("tools", package.seeall)

function deepCopy(object)
	local copied = {}
	local function _copy(object)
		if type(object) ~= "table" or object.__ref then
			return object
		elseif copied[object] then
			return copied[object]
		end
		
		local newTable = {}
		copied[object] = newTable
		
		for k, v in pairs(object) do
			newTable[_copy(k)] = _copy(v)
		end
		return setmetatable(newTable, getmetatable(object))
	end
	return _copy(object)
end