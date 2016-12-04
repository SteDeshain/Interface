welcomeScene = {}
module("welcomeScene", package.seeall)
require "scripts/scenes/scene"

scene:new(welcomeScene)
function welcomeScene:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

local function welcomeSceneInit(ud)
	--...
	print("welcomeScene!")
end

local function welcomeSceneUpdate(ud, dt)
	--...
end

-- 英文实在憋不出来了，就用中文吧
-- "length"有助于c++代码处理循环次数
-- 每一项中都有"name"字段，在该项构建后，会在本sources表中保存一个以相应name为key的字段，存放相应的userdata（也可以考虑连同描述class的字符串一并存进去）
-- 因此各项的顺序尤为重要，以防止后面的项在构建时找不到相应依赖的userdata
-- 在一项构建完成后，可删除相应参数表
-- 在构建每一项时，根据"class"字段判断对象的类，以调用正确的构造函数，但在c++代码中，每一个变量都是GameComp*，但由于多态的便捷，会将不同类型的GameComp储存到正确的list中去
welcomeScene.sources = {}
welcomeScene.sources["length"] = 4
welcomeScene.sources[1] =
{
	["name"] = "canvas_1",
	["class"] = "Canvas",
	["length"] = 5,
	[1] = {255, 255, 255, 255},		-- color: white
	[2] = 0.0,						-- transparency: 0.0f
	[3] = {0, 0, Config.window.width, Config.window.height},
									-- viewRect: whole window
	[4] = {0, 0, Config.window.width, Config.window.height},
									-- canvasSize: whole window
	[5] = nil,						-- motherCanvas: NULL
}
welcomeScene.sources[2] =
{
	["name"] = "gui_1",
	["class"] = "GUI",
	["length"] = 5,
	[1] = Config.window.width / 2,		-- x
	[2] = Config.window.height / 2,		-- y
	[3] = 1,							-- textureNum
	[4] = "sheet.png",					-- fileName
	[5] = welcomeScene.sources["canvas_1"],
										-- motherCanvas: canvas_1 (when constructing this gui, the canvas_1 already becomes a userdata pointing to the correct object)
}

welcomeScene.sources[3] =
{
	["name"] = "canvas_2",
	["class"] = "Canvas",
	["length"] = 5,
	[1] = {255, 255, 255, 255},
	[2] = 0.0,
	[3] = {0, 0, Config.window.width, Config.window.height},
	[4] = {0, 0, Config.window.width, Config.window.height},
	[5] = nil,
}
welcomeScene.sources[4] =
{
	["name"] = "gui_2",
	["class"] = "GUI",
	["length"] = 5,
	[1] = Config.window.width / 2,
	[2] = Config.window.height / 2,
	[3] = 1,
	[4] = "tiles.png",
	[5] = welcomeScene.sources["canvas_2"],
}

-- interface
function welcomeScene.init()
	--...
	return
		welcomeSceneInit,
		1,
		0
end

function welcomeScene.update()
	--...
	return
		welcomeSceneUpdate,
		2,
		0
end
