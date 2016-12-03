scene = {}
module("scene", package.seeall)

function scene:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

local function sceneInit(ud)
	--...
end

local function sceneUpdate(ud, dt)
	--...
end

scene.sources = {}	-- GameComp array!!! so its elements are lightuserdata
-- every element's key is a name(a string)
-- at first, every element's value is a table containing argument of construcor to create a GameComp
-- after constructor finished, each element's table is discarded, and the pointer replaced it as a userdata

-- interface
function scene.init()
	--...
	return
		sceneInit,
		1,
		0
end

function scene.update()
	--...
	return
		sceneUpdate,
		2,
		0
end
