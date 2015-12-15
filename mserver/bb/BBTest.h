//
//  BBTest.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 6/4/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#ifndef __VoxipBBTests__BBTest__
#define __VoxipBBTests__BBTest__

#include "common_headers.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::WithArgs;
using ::testing::Invoke;


//======================================================================================================================
// This macro is a replacement for gtest TEST_F macro. Instead of defining the TestBody() method, it defines the
// BBTest::InternalTestBody() method. This is to enable having a fixed structure to TestBody() method for all tests,
// which will do some common stuff and call InternalTestBody().
// It is still possible to use TEST_F. If doing so, the default implementation of TestBody() will be overriden.
//======================================================================================================================
#define BB_TEST_F(test_fixture, test_name)\
BB_TEST_F_(test_fixture, test_name, test_fixture, ::testing::internal::GetTypeId<test_fixture>())

#define BB_TEST_F_(test_case_name, test_name, parent_class, parent_id)\
class GTEST_TEST_CLASS_NAME_(test_case_name, test_name): public parent_class {\
public:\
    GTEST_TEST_CLASS_NAME_(test_case_name, test_name)() {}\
private:\
    virtual void internal_test_body();\
    static ::testing::TestInfo* const test_info_ GTEST_ATTRIBUTE_UNUSED_;\
    GTEST_DISALLOW_COPY_AND_ASSIGN_(\
        GTEST_TEST_CLASS_NAME_(test_case_name, test_name));\
};\
\
::testing::TestInfo* const GTEST_TEST_CLASS_NAME_(test_case_name, test_name)::test_info_ =\
    ::testing::internal::MakeAndRegisterTestInfo(\
        #test_case_name, #test_name, NULL, NULL, (parent_id), parent_class::SetUpTestCase,\
        parent_class::TearDownTestCase,\
        new ::testing::internal::TestFactoryImpl<GTEST_TEST_CLASS_NAME_(test_case_name, test_name)>);\
void GTEST_TEST_CLASS_NAME_(test_case_name, test_name)::internal_test_body()


//======================================================================================================================
// This type is for distingushing between different kind of abort, i.e. killing of external processes.
//======================================================================================================================
enum BBAbortKind {SOFT, HARD};

//======================================================================================================================
// Struct used to communicate with BBTest::launch_command() "thread". If caller sets member <abort> launch_command() will
// terminate its external process. <is_running> is used to indicate that the process is running.
//======================================================================================================================
template <class T>
class LaunchInfo
{
public:
    //------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------
    LaunchInfo(string _name):
        name(_name), delay(0), is_running(false), abort(false), abort_kind(HARD), successful(true), timeout(0) {}

    //------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------
    LaunchInfo(string _name, int _delay, bool _isRunning):
        name(_name), delay(_delay), is_running(_isRunning), abort(false), abort_kind(HARD), successful(true), timeout(0) {}

    //------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------
    void soft_abort()
    {
        abort_kind = SOFT;
        abort = true;
        wait_false(abort);
    }

    string name;            // Name or short description of the process associated with this object
    int delay;              // Number of seconds to wait before launching the process, default is 0
    bool is_running;        // Indicates external process is running
    bool abort;             // Tell the launch_command() function to kill the external process
    BBAbortKind abort_kind; // What kind of abort
    bool successful;        // true if external process succeeded, false o/w
    future<T> future_res;   // Future result object, used by async function
    int timeout;            // Timeout to be used when waiting for the process to finish
};

typedef LaunchInfo<void> VoidLaunchInfo;

//======================================================================================================================
// Base for all test classes
//======================================================================================================================
class BBTest: public testing::Test
{
public:
    virtual void TestBody();
    virtual void internal_test_body() = 0;
    virtual void post_internal_test_body(){} // Hook for doing generic per class checks after internal_test_body()
    virtual void SetUp();
    virtual void TearDown();
    virtual void config(string port);

    virtual void voxip_ui_callback(EVoXIPUIEvent event, const char* data);

    //------------------------------------------------------------------------------------------------------------------
    // General external processes methods
    //------------------------------------------------------------------------------------------------------------------
    void launch_proxy(VoidLaunchInfo *launch_info);
    int count_num_tcp_connection_attempts(string interface, string port, LaunchInfo<int> *launch_info);

    //------------------------------------------------------------------------------------------------------------------
    // SIPP scenarios launchers
    //------------------------------------------------------------------------------------------------------------------
    void launch_sipp_server(VoidLaunchInfo *launch_info, string script);
    void launch_sipp_server_full(VoidLaunchInfo *launch_info, string script, string params_file, string extra_parmas = "");
    void launch_sipp_client(VoidLaunchInfo *launch_info, string script);
    void launch_sipp_client(VoidLaunchInfo *launch_info, string script, string params_file, string extra_params = "");
    void launch_sipp_for_registration(VoidLaunchInfo *launch_info);
    void launch_sipp_for_registration_fail(VoidLaunchInfo *launch_info, string error_code);

protected:
    vector<VoidLaunchInfo*> launch_infos;
    
    bool dont_check_mserver = false;
    void run_mserver_scenario(string filename, string params = "");
    void check_mserver_status();

    //------------------------------------------------------------------------------------------------------------------
    // General external processes methods
    //------------------------------------------------------------------------------------------------------------------
    void launch_command(string command, VoidLaunchInfo *launch_info = nullptr);
    void launch_command(char *const argv[], VoidLaunchInfo *launch_info = nullptr);

    void create_fail_script(string script_template, string error_code, string &script_path);
    string template_to_actual_script_name(string script_template);

    //------------------------------------------------------------------------------------------------------------------
    // SIPP scenarios launchers
    //------------------------------------------------------------------------------------------------------------------
    string build_sipp_command(string script, string port, string params_file, SippMode sipp_mode, string extra_params = "");
    string build_sipp_command(string script, string port, string params_file);
    string build_sipp_command(string script, string port, SippMode sipp_mode);
    string build_sipp_command(string script, string port);

    //------------------------------------------------------------------------------------------------------------------
    // Methods for activating voxip
    //------------------------------------------------------------------------------------------------------------------
    void register_voxip(int timeout = 5, bool throwIfFails = true);
    void unregister_voxip();


    //------------------------------------------------------------------------------------------------------------------
    // General utility methods
    //------------------------------------------------------------------------------------------------------------------
    void wait_for_process_to_run(VoidLaunchInfo *launch_info, int timeout);
    void get_future_res(VoidLaunchInfo *launch_info);
    void kill_processes(int timeout = -1);

private:
    static void kill_child_process(pid_t pid);
};

#endif /* defined(__VoxipBBTests__BBTest__) */



































