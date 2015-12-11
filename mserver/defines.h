//
//  defines.h
//  mserver
//
//  Created by Yuval Dinari on 11/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef defines_h
#define defines_h

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
#define MEDIA_IP_TYPE   "media_ip_type"
#define MEDIA_IP        "media_ip"
#define MEDIA_PORT      "media_port"
#define DEFAULT_RESPONSE_BODY   "default_response_body"
#define DEFAULT_REQUEST_BODY    "default_request_body"
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

#endif /* defines_h */
