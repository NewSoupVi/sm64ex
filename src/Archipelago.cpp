#include "Archipelago.h"

#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXUserAgent.h"

extern "C" {
    #include "game/print.h"
}

#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <deque>
#include <string>
#include <chrono>
#include <cstdlib>

#define ADD_TO_MSGQUEUE(x,y) messageQueue.push_back(std::pair<std::string,int>(x,y))

bool init = false;
bool auth = false;
int sm64ap_player_id;
std::string sm64ap_player_name;
std::string sm64ap_ip;
std::string sm64ap_passwd;
int sm64ap_uuid = 0;
int starsCollected = 0;
bool sm64_have_key1 = false;
bool sm64_have_key2 = false;
int msg_frame_duration = 90; // 4 Secounds at 30F/s
int cur_msg_frame_duration = msg_frame_duration; 
std::deque<std::pair<std::string,int>> messageQueue;
std::map<int, std::string> map_player_id_name;
std::map<int, std::string> map_location_id_name;
std::map<int, std::string> map_item_id_name;

ix::WebSocket webSocket;
Json::Reader reader;
Json::FastWriter writer;

bool parse_response(std::string msg, std::string &request);
void APSend(std::string req);

void SM64AP_Init(const char* ip, const char* player_name, const char* passwd) {
    if (init) {
        return;
    }

    std::srand(std::time(nullptr)); // use current time as seed for random generator

    sm64ap_player_name = std::string(player_name);
    sm64ap_ip = std::string(ip);
    sm64ap_passwd = std::string(passwd);

    printf("SM64AP: Initializing...\n");

    //Connect to server
    ix::initNetSystem();
    std::string url("ws://" + sm64ap_ip);
    webSocket.setUrl(url);
    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                std::string request;
                if (parse_response(msg->str, request)) {
                    APSend(request);
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Open)
            {
                printf("SM64AP: Connected to Archipelago\n");
            }
            else if (msg->type == ix::WebSocketMessageType::Error)
            {
                auth = false;
                printf("SM64AP: Error connecting to Archipelago. Retries: %d\n", msg->errorInfo.retries-1);
            }
        }
    );

    init = true;
    webSocket.start();
}

void SM64AP_SendItem(int idx) {
    if (!auth) {
        printf("SM64AP: Something funky is happening...\n");
        return;
    }
    printf(("SM64AP: Checked " + map_location_id_name.at(idx + SM64AP_ID_OFFSET) + ". Informing Archipelago...\n").c_str());
    Json::Value req_t;
    req_t[0]["cmd"] = "LocationChecks";
    req_t[0]["locations"][0] = idx + SM64AP_ID_OFFSET;
    APSend(writer.write(req_t));
}

void SM64AP_StoryComplete() {
    if (auth) {
        Json::Value req_t;
        req_t[0]["cmd"] = "StatusUpdate";
        req_t[0]["status"] = 30; //CLIENT_GOAL
        APSend(writer.write(req_t));
    }
}

int SM64AP_GetStars() {
    return starsCollected;
}

bool SM64AP_HaveKey1() {
    return sm64_have_key1;
}

bool SM64AP_HaveKey2() {
    return sm64_have_key2;
}

bool SM64AP_DeathLinkRecv() {
    return false;
}

void SM64AP_DeathLinkClear() {

}

void SM64AP_DeathLinkSend() {

}

void SM64AP_PrintNext() {
    if (messageQueue.empty()) return;
    int amount = messageQueue.front().second;
    for (int i = 0; i <= amount; i++) {
        print_text(-60, (amount-i)*20, messageQueue.at(i).first.c_str());
    }
    if (cur_msg_frame_duration > 0) {
        cur_msg_frame_duration--;
    } else {
        for (int i = 0; i <= amount; i++) {
            messageQueue.pop_front();
        }
        messageQueue.shrink_to_fit();
        cur_msg_frame_duration = msg_frame_duration;
    }
}

bool parse_response(std::string msg, std::string &request) {
    Json::Value root;
    reader.parse(msg, root);
    for (unsigned int i = 0; i < root.size(); i++) {
        const char* cmd = root[0]["cmd"].asCString();
        if (!strcmp(cmd,"RoomInfo")) {
            if (!auth) {
                Json::Value req_t;
                sm64ap_uuid = std::rand();
                req_t[i]["cmd"] = "Connect";
                req_t[i]["game"] = "Super Mario 64";
                req_t[i]["name"] = sm64ap_player_name;
                req_t[i]["password"] = sm64ap_passwd;
                req_t[i]["uuid"] = sm64ap_uuid;
                req_t[i]["tags"][0] = "DeathLink"; // Send Tag even though we don't know if we want these packages, just in case
                req_t[i]["version"]["major"] = "0";
                req_t[i]["version"]["minor"] = "2";
                req_t[i]["version"]["build"] = "2";
                req_t[i]["version"]["class"] = "Version";
                request = writer.write(req_t);
                auth = false;
                return true;
            }
        } else if (!strcmp(cmd,"Connected")) {
            // Avoid inconsistency if we disconnected before
            sm64_have_key1 = false;
            sm64_have_key2 = false;
            starsCollected = 0;

            printf("SM64AP: Authenticated\n");
            sm64ap_player_id = root[i]["slot"].asInt();
            for (unsigned int j = 0; j < root[i]["checked_locations"].size(); j++) {
                //Sync checks with server
                int loc_id = root[i]["checked_locations"][j].asInt() - SM64AP_ID_OFFSET;
                //save_file_set_star_flags(int fileIndex, int courseIndex, int starFlags)
            }
            for (unsigned int j = 0; j < root[i]["players"].size(); j++) {
                map_player_id_name.insert(std::pair<int,std::string>(root[i]["players"][j]["slot"].asInt(),root[i]["players"][j]["alias"].asString()));
            }
            Json::Value req_t;
            req_t[0]["cmd"] = "GetDataPackage";
            request = writer.write(req_t);
            return true;
        } else if (!strcmp(cmd,"DataPackage")) {
            for (unsigned int j = 0; j < root[i]["data"]["games"].size(); j++) {
                for (auto itr : root[i]["data"]["games"]) {
                    for (auto itr2 : itr["item_name_to_id"].getMemberNames()) {
                        map_item_id_name.insert(std::pair<int,std::string>(itr["item_name_to_id"][itr2.c_str()].asInt(), itr2));
                    }
                    for (auto itr2 : itr["location_name_to_id"].getMemberNames()) {
                        map_location_id_name.insert(std::pair<int,std::string>(itr["location_name_to_id"][itr2.c_str()].asInt(), itr2));
                    }
                }
            }
            Json::Value req_t;
            req_t[0]["cmd"] = "Sync";
            request = writer.write(req_t);
            auth = true;
            return true;
        } else if (!strcmp(cmd,"Print")) {
            printf("AP: %s\n", root[i]["text"].asCString());
        } else if (!strcmp(cmd,"PrintJSON")) {
            if (!strcmp(root[i].get("type","").asCString(),"ItemSend")) {
                if (map_player_id_name.at(root[i]["item"]["player"].asInt()) != sm64ap_player_name) continue;
                ADD_TO_MSGQUEUE((map_item_id_name.at(root[i]["item"]["item"].asInt()) + " was sent"), 1);
                ADD_TO_MSGQUEUE(("to " + map_player_id_name.at(root[i]["receiving"].asInt())), 0);
                printf("Item from %s to %s\n", map_player_id_name.at(root[i]["item"]["player"].asInt()).c_str(), map_player_id_name.at(root[i]["receiving"].asInt()).c_str());
            } else if(!strcmp(root[i].get("type","").asCString(),"Hint")) {
                printf("Hint: Item %s from %s to %s at %s %s\n", map_item_id_name.at(root[i]["item"]["item"].asInt()).c_str(), map_player_id_name.at(root[i]["item"]["player"].asInt()).c_str(),
                                                                 map_player_id_name.at(root[i]["receiving"].asInt()).c_str(), map_location_id_name.at(root[i]["item"]["location"].asInt()).c_str(),
                                                                 (root[i]["found"].asBool() ? " (Checked)" : " (Unchecked)"));
            }
        } else if (!strcmp(cmd, "LocationInfo")) {
            //Uninteresting for now.
        } else if (!strcmp(cmd, "ReceivedItems")) {
            for (unsigned int j = 0; j < root[i]["items"].size(); j++) {
                int item_id = root[i]["items"][j]["item"].asInt();
                switch (item_id) {
                    case SM64AP_ITEMID_STAR:
                        starsCollected++;
                        break;
                    case SM64AP_ITEMID_KEY1:
                        sm64_have_key1 = true;
                        break;
                    case SM64AP_ITEMID_KEY2:
                        sm64_have_key2 = true;
                        break;
                }
            }
        } else if (!strcmp(cmd, "RoomUpdate")) {
            for (unsigned int j = 0; j < root[i]["checked_locations"].size(); j++) {
                //Sync checks with server
                int loc_id = root[i]["checked_locations"][j].asInt() - SM64AP_ID_OFFSET;
                #warning TODO Sync Checks
            }
        } else if (!strcmp(cmd, "ConnectionRefused")) {
            auth = false;
            printf("SM64AP: Archipelago Server has refused connection. Check Password / Name / IP and restart the Game.");
            webSocket.stop();
        } else if (!strcmp(cmd, "Bounced")) {
            // None expected. Ignoring
        }
        
        else {
            printf("SM64AP: Unhandled Packet. Command: %s\n", cmd);
        }
    }
    return false;
}

void APSend(std::string req) {
    webSocket.send(req);
}