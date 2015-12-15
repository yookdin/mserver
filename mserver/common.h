#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <stack>
#include <string>
#include <regex>
#include <queue>
#include <poll.h>
#include <cmath>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

#include "mserver_utils.hpp"

using namespace std;

//==========================================================================================================
// Global variable names
//==========================================================================================================
#define RUN_DIR         "run_dir"
#define SCENARIO_DIR    "scenarios_dir"
#define IPS             "ips"
#define SERVER_IP       "server_ip"
#define SERVER_PORT     "server_port"
#define CONTROL_PORT    "control_port"
#define TEST_DIR        "test_dir"
#define SCENARIO        "scenario"
#define CLIENT_IP       "client_ip"
#define CLIENT_PORT     "client_port"
#define TRANSPORT       "transport"
#define PID             "pid"
#define SIP_IP_TYPE     "sip_ip_type"
#define CLIENT_IP_TYPE  "client_ip_type"

#define AUDIO_IP_TYPE   "audio_ip_type"
#define AUDIO_IP        "audio_ip"
#define AUDIO_PORT      "audio_port"

#define VIDEO_IP_TYPE   "video_ip_type"
#define VIDEO_IP        "video_ip"
#define VIDEO_PORT      "video_port"

#define DEFAULT_RESPONSE_BODY   "default_response_body"
#define DEFAULT_REQUEST_BODY    "default_request_body"
#define DEFAULT_VIDEO_RESPONSE_BODY   "default_video_response_body"
#define DEFAULT_VIDEO_REQUEST_BODY    "default_video_request_body"
#define DEFAULT_100_TRYING      "default_100_trying"
#define DEFAULT_ACK             "default_ack"

//==========================================================================================================
// Generative variables
//==========================================================================================================
#define BRANCH          "branch"
#define CALL_ID         "call_id"
#define MIN_CALL_ID     "min_call_id"
#define MAX_CALL_ID     "max_call_id"
#define LEN             "len"
#define TAG             "tag"

//==========================================================================================================
// Message variable names
//==========================================================================================================
#define CSEQ            "cseq"
#define SDP_SEND_RECV   "sdp_send_recv"
#define ME              "me"
#define OTHER           "other"


//==========================================================================================================
// Constants
//==========================================================================================================
#define CONNECTION_BUFFER_SIZE 5000
#define CONTROL_BUFFER_SIZE 500
#define CRLF "\r\n"
