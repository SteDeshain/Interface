-- no use anymore

cobj = {}

function cobj:new(ud, class, o)
	-- check argument
	if type(ud) ~= "userdata" then
		error("argument \"ud\" must be a userdata!");
	end
	
	if type(class) ~= "string" then
		error("argument \"class\" must be a string!");
	end
	
	o = o or {}
	if type(o) ~= "table" then
		error("argument \"o\" must be a table or nil!");
	end
	
	o.ud = ud		-- the pointer pointing to the c++ object
	o.class = class	-- the class of the object
	return o
end