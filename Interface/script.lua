Script = {}

print"hello lua module!"

s = "it's a lua string"

t = true

a = 100

b = 2.3

funcs = {}
function funcs.foo()
    return
        function(a)
            return a * 2
        end,
        1,
        1
end

--ud = 0
--lud = 0

t1 = {}
t1.t2 = {}
t1.t2.t3 = {}
t1.t2.t3.v = 100
t1.t2.t3.foo = function()
    return
        function()
            print("succeed!!!")
            return
                1.5,
                nil,
                {},
                function() return 1 end,
                "a lua string returned by lua function.",
                true,
                ud,
                lud
        end,
        0,
        8
end

