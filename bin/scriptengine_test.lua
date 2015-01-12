-- scriptengine_test.lua

function TestMD5()
 local md5 = MD5String("test")
 print(md5)
end


function DoMain()
 TestMD5()
 return 0
end
