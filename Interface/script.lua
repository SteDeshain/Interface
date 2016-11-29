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

t1 = {}
t1.t2 = {}
t1.t2.t3 = {}
t1.t2.t3.v = 100
