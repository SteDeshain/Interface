welcomeScene = {}
module("welcomeScene", package.seeall)
require "scenes/scene"
require "tools"

scene:new(welcomeScene)
function welcomeScene:new(o, ud)
	o = o or {}
	setmetatable(o, self)
	self.__index = function(table, key)
		local value = self[key]
		table[key] = tools.deepCopy(value)
		return table[key]
	end
	
	if ud ~= nil then
		if type(ud) ~= "userdata" then
			error("argument ud must be a userdata!")
		else
			o.ud = ud
		end
	end

	return o
end

welcomeScene.New = 
{
	["function"] = welcomeScene.new,
	["argNumber"] = 3,
	["resNumber"] = 1,
}

--temp
welcomeScene.n = 0

function welcomeScene:update(dt)
	--...
	self.n = self.n + dt
	if self.n >= 2000 then
		print("2 seconds..." .. self.n)
		self.n = 0
	end
end

welcomeScene.Update = 
{
	["function"] = welcomeScene.update,
	["argNumber"] = 2,
	["resNumber"] = 0,
}

-- 英文实在憋不出来了
-- "length"有助于c++代码处理循环次数
-- 每一项中都有"name"字段，在该项构建后，会在本sources表中保存一个以相应name为key的字段，
-- 对应value为一个luaProxy的table(其实该luaProxy已在全局变量中注册过，不过全局的key的名字可能和sources中的不一样)，来存放相应的对象（也可以考虑连同描述class的字符串一并存进去???）
-- 因此各项的顺序尤为重要，以防止后面的项在构建时找不到相应依赖的userdata
-- 在一项构建完成后，可删除相应参数表
-- 在构建每一项时，根据"class"字段判断对象的类，以调用正确的构造函数，但在c++代码中，每一个变量都是GameComp*，但由于多态的便捷，会将不同类型的GameComp储存到正确的list中去
welcomeScene.sources = {}
welcomeScene.sources["length"] = 4

welcomeScene.sources["canvas_1"] = {}
welcomeScene.sources[1] =
{
	["name"] = "canvas_1",	--脚本的编写者只负责在sources表中的命名不冲突，这对脚本编写者来说足够了
							--但在全局变量中，相应的luaProxy的key很有可能和sources表中的key不同，因为在这里该命名可能不冲突，但放在全局中可能和其他的luaProxy冲突
							--所以在c++代码中，会处理以防全局中的命名冲突
	["class"] = "Canvas",
	["length"] = 7,
	[1] = "color",					-- color mode
	[2] = "logoCanv",				-- no script
	[3] = {0, 128, 255, 255},		-- color: white
	[4] = 0.8,						-- transparency: 0.0f
---[[
	[5] = {0, 0, Config.window.width, Config.window.height},
									-- viewRect: whole window
	[6] = {Config.window.width, Config.window.height},
									-- canvasSize: whole window
--]]
	[7] = nil,						-- motherCanvas: NULL
	[8] = false,					-- startVisible
}
welcomeScene.sources["gui_1"] = {}
welcomeScene.sources[2] =
{
	["name"] = "gui_1",
	["class"] = "GUI",
	["length"] = 8,
	[1] = "picture",					-- mode, not used in constructor, used in c++ to decide to use which constructor
	[2] = nil,							-- name, nil for no script GameComp
	[3] = Config.window.width / 2,		-- x
	[4] = Config.window.height / 2,		-- y
	[5] = 1,							-- textureNum
	[6] = 1,							-- textureNum
	[7] = "sheet.png",					-- fileName
	[8] = welcomeScene.sources["canvas_1"].ud,
										-- motherCanvas: canvas_1 (when constructing this gui, the canvas_1 already became a luaProxy pointing to the correct object)
}

welcomeScene.sources["canvas_2"] = {}
welcomeScene.sources[3] =
{
	["name"] = "canvas_2",
	["class"] = "Canvas",
	["length"] = 7,
	[1] = "color",
	[2] = "logoCanv",
	[3] = {0, 0, 250, 255},
	[4] = 0.9,
	[5] = {100, 100, 100, 100},
	[6] = {100, 101},
	[7] = nil,
	[8] = true,
}
welcomeScene.sources["gui_2"] = {}
welcomeScene.sources[4] =
{
	["name"] = "gui_2",
	["class"] = "GUI",
	["length"] = 8,
	[1] = "picture",
	[2] = nil,
	[3] = Config.window.width / 2,
	[4] = Config.window.height / 2,
	[5] = 1,
	[6] = 1,
	[7] = "tiles.png",
	[8] = welcomeScene.sources["canvas_2"].ud,
}
