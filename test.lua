failed_tests = {}
SERVER = true
GetConVar("gluatest_enable"):SetBool(true)

hook.Add("GLuaTest_LoggedTestFailure", "record_end_result", function(errInfo)
    table.insert(failed_tests, errInfo)
end)

GLuaTest.runAllTests()

if #failed_tests > 0 then
    print("Failed " .. #failed_tests .. " tests:")
    for _, info in ipairs(failed_tests) do
        print(" - " .. info.sourceFile .. ":" .. info.lineNumber .. ": " .. info.reason)
    end
    return 1
end
