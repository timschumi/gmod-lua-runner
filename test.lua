runner.return_value = 1
found_failed_test = false
SERVER = true
GetConVar("gluatest_enable"):SetBool(true)

hook.Add("GLuaTest_LoggedTestFailure", "record_test_failures", function(errInfo)
    found_failed_test = true
end)

hook.Add("GLuaTest_Finished", "report_end_result", function()
    if not found_failed_test then
        runner.return_value = 0
    end
end)

hook.Run("Tick")
