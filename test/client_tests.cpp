#include <catch2/catch_test_macros.hpp>
#include <string>
#include "../src/client.h"
#include <jsoncpp/json/json.h>

#define CONFIG "../../.api_keys/riot_config.json", "../test/test_log.txt", true, true

using namespace client;

TEST_CASE( "TESTING ACCOUNT_V1 QUERIES") {
    RiotApiClient test_client(CONFIG);

    std::vector<std::string> region = {"americas", "asia", "europe"};
    std::string puuid = "nvf_tmMcUOXhaWI4neMvma8WG3iymFlqS32-FpUFsLk3FqH9pGB_oQh5DoecV-Uiue2NIYddgww50A";
    std::string gamename = "DanTan1"; std::string tagline = "fresn";
    std::string game = "val";

    std::string endpoint = "ACCOUNT-V1";

    Json::Value result;
    
    for (int i = 0; i < 3; i++) {
        result = test_client.query(endpoint, std::string("by-puuid"), std::vector<std::string>{region[i], puuid});
        REQUIRE(result["puuid"] == puuid);
        result = test_client.query(endpoint, std::string("by-riot-id"), std::vector<std::string>{region[i], gamename, tagline});
        REQUIRE(result["gameName"] == gamename);
        REQUIRE(result["tagLine"] == tagline);
        result = test_client.query(endpoint, std::string("by-game"), std::vector<std::string>{region[i], game, puuid});
        REQUIRE(result["puuid"] == puuid);
    }
}

// League of Legend Queries
//
//TEST_CASE( "TESTING LEAGUE_V4 QUERIES") {
//    RiotApiClient test_client(CONFIG);
//    
//    std::string region = "na1";
//    std::vector<std::string> queue = {"RANKED_SOLO_5x5", "RANKED_FLEX_SR"};
//    std::vector<std::string> division = {"I", "II", "III", "IV"};
//    std::vector<std::string> tier = {"DIAMOND", "PLATINUM", "GOLD", "SILVER", "BRONZE", "IRON"};
//    std::string summoner_id = "Ob0sRhCSzqtSRNHfAk_kS6Ac9TNOzKVhqj_kYr74HOhf2II";
//    std::string league_id = "16dfc561-5064-4a5d-b4c4-9d4bfa03773f";
//
//    Json::Value result;
//
//    SECTION("Testing Challenger, Grandmaster and Master queue queries") {
//        for (int i = 0; i < queue.size(); i++) {
//            result = test_client.LEAGUE_V4_challenger(queue[i], region);
//            REQUIRE(result["tier"] == "CHALLENGER");
//            REQUIRE(result["queue"] == queue[i]);
//            result = test_client.LEAGUE_V4_grandmaster(queue[i], region);
//            REQUIRE(result["tier"] == "GRANDMASTER");
//            REQUIRE(result["queue"] == queue[i]);
//            result = test_client.LEAGUE_V4_master(queue[i], region);
//            REQUIRE(result["tier"] == "MASTER");
//            REQUIRE(result["queue"] == queue[i]);
//        }
//    }
//
//    SECTION("Testing specific queue queries") {
//        for (int qu = 0; qu < queue.size(); qu++) {
//            for (int ti = 0; ti < tier.size(); ti++) {
//                for (int div = 0; div < division.size(); div++) {
//                    result = test_client.LEAGUE_V4_queue(queue[qu], tier[ti], division[div], region);
//                    REQUIRE(result[0]["queueType"] == queue[qu]);
//                    REQUIRE(result[0]["tier"] == tier[ti]);
//                    REQUIRE(result[0]["rank"] == division[div]);
//                }
//            }
//        }
//    }
//
//    SECTION("Testing Summoner ID and League ID") {
//        result = test_client.LEAGUE_V4_summonerid(summoner_id, region);
//        REQUIRE(result[0]["summonerId"] == summoner_id);
//        result = test_client.LEAGUE_V4_leagueid(league_id, region);
//        REQUIRE(result["leagueId"] == league_id);
//    }
//}
//
//TEST_CASE(" TESTING SUMMONER QUERIES", "[RiotApiClient::SUMMONER_V4]") {
//    RiotApiClient test_client(CONFIG);
//    
//    std::string region = "kr";
//    std::string account_id = "3Y8PsnoE72WOCKHi3AJ8VydiBZLPmY2LHoQYYZmz_G20";
//    std::string summoner_name = "Hide on bush";
//    std::string puuid = "6dgDp5y88RxqOmVMv1GRoGaCmPP-uAbmlsVRhKQj4g0KdIH_GxqCEE6w0JRmHRxSTzbtxMFGypJZIg";
//    std::string summoner_id = "migc0LCQpHU_dwE38S5HIA5VTDoNgR-rDEE8_iC5CJ0GHA";
//
//    Json::Value result;
//
//    result = test_client.SUMMONER_V4_accountid(account_id, region);
//    REQUIRE(result["accountId"] == account_id);
//    result = test_client.SUMMONER_V4_summoner_name(summoner_name, region);
//    REQUIRE(result["name"] == summoner_name);
//    result = test_client.SUMMONER_V4_puuid(puuid, region);
//    REQUIRE(result["puuid"] == puuid);
//    result = test_client.SUMMONER_V4_summonerid(summoner_id, region);
//    REQUIRE(result["id"] == summoner_id);
//
//}
//
//TEST_CASE( "TESTING MATCH QUERIES", "[RiotApiClient::MATCH_V5]" ) {
//    RiotApiClient test_client(CONFIG);
//
//    std::string region = "asia";
//    std::string puuid = "6dgDp5y88RxqOmVMv1GRoGaCmPP-uAbmlsVRhKQj4g0KdIH_GxqCEE6w0JRmHRxSTzbtxMFGypJZIg";
//    std::string match_id = "KR_6279823690";
//
//    Json::Value result;
//
//    result = test_client.MATCH_V5_puuid(puuid, region);
//    result = test_client.MATCH_V5_matchid(match_id, region);
//    REQUIRE(result["metadata"]["matchId"] == match_id);
//    result = test_client.MATCH_V5_timeline(match_id, region);
//    REQUIRE(result["metadata"]["matchId"] == match_id);
//
//}
//
////TEST_CASE( "TESTING CLASH QUERIES") {
////    RiotApiClient test_client(KEY_PATH, "not_required");
////
////    std::string region = "";
////    std::string summoner_id = "";
////    std::string team_id = "";
////    std::string tournaments = "";
////
////    Json::Value result;
////
////    result = test_client.CLASH_V1_summonerid(summoner_id, region);
////    result = test_client.CLASH_V1_teamid(team_id, region);
////    result = test_client.CLASH_V1_tournament_byteam(team_id, region);
////    result = test_client.CLASH_V1_tournamentid(tournament_id, region);
////
//}

// TODO: 

// Legends of Runeterra Queries

// Valorant Queries
