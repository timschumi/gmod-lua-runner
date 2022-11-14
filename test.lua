runner.return_value = 1
failed_tests = {}
SERVER = true
GetConVar("gluatest_enable"):SetBool(true)

hook.Add("GLuaTest_LoggedTestFailure", "record_test_failures", function(errInfo)
    table.insert(failed_tests, errInfo)
end)

hook.Add("GLuaTest_Finished", "report_end_result", function()
    if #failed_tests == 0 then
        runner.return_value = 0
        return
    end

    print("Failed " .. #failed_tests .. " tests:")
    for _, info in ipairs(failed_tests) do
        print(" - " .. info.sourceFile .. ":" .. info.lineNumber .. ": " .. info.reason)
    end
end)

hook.Run("Tick")
