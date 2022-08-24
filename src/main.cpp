#include "Broadcaster.hpp"
#include "MediaStreamTrackFactory.hpp"
#include "mediasoupclient.hpp"
#include "pc/test/frame_generator_capturer_video_track_source.h"
#include <cpr/cpr.h>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using json = nlohmann::json;
std::vector<std::thread> workers;

std::vector<Broadcaster*> LBroadcaster;

void signalHandler(int signum)
{
	for (Broadcaster* b : LBroadcaster)
	{
		b->Stop();
		delete b;
	}
	std::cout << "[INFO] interrupt signal (" << signum << ") received" << std::endl;

	std::cout << "[INFO] leaving!" << std::endl;

	std::exit(signum);
}

int main(int /*argc*/, char* /*argv*/[])
{
	// Register signal SIGINT and signal handler.
	signal(SIGINT, signalHandler);
	// Retrieve configuration from environment variables.
	const char* envServerUrl    = std::getenv("SERVER_URL");
	const char* envRoomId       = std::getenv("ROOM_ID");
	const char* envEnableAudio  = std::getenv("ENABLE_AUDIO");
	const char* envUseSimulcast = std::getenv("USE_SIMULCAST");
	const char* envWebrtcDebug  = std::getenv("WEBRTC_DEBUG");
	const char* envVerifySsl    = std::getenv("VERIFY_SSL");
	const char* numberClient    = std::getenv("NUMBER_CLIENT");
	const char* numberSquare    = std::getenv("NUMBER_SQUARE");
	const char* widthSize       = std::getenv("WIDTH_SIZE");
	const char* heightSize      = std::getenv("HEIGHT_SIZE");
	const char* runParallel     = std::getenv("RUN_PARALLEL");

	if (envServerUrl == nullptr)
	{
		std::cerr << "[ERROR] missing 'SERVER_URL' environment variable" << std::endl;

		return 1;
	}

	if (envRoomId == nullptr)
	{
		std::cerr << "[ERROR] missing 'ROOM_ID' environment variable" << std::endl;

		return 1;
	}

	if (numberClient == nullptr)
	{
		std::cerr << "[ERROR] missing 'NUMBER_CLIENT' environment variable" << std::endl;

		return 1;
	}
	if (numberSquare == nullptr)
	{
		std::cerr << "[ERROR] missing 'NUMBER_SQUARE' environment variable" << std::endl;

		return 1;
	}

	if (widthSize == nullptr)
	{
		std::cerr << "[ERROR] missing 'WIDTH_SIZE' environment variable" << std::endl;

		return 1;
	}
	if (heightSize == nullptr)
	{
		std::cerr << "[ERROR] missing 'HEIGHT_SIZE' environment variable" << std::endl;

		return 1;
	}
	if (runParallel == nullptr)
	{
		std::cerr << "[ERROR] missing 'RUN_PARALLEL' environment variable" << std::endl;

		return 1;
	}
	bool enableAudio   = true;
	bool isRunParallel = false;

	if (std::string(runParallel) == "true")
		isRunParallel = true;

	if (envEnableAudio && std::string(envEnableAudio) == "false")
		enableAudio = false;

	bool useSimulcast = true;

	if (envUseSimulcast && std::string(envUseSimulcast) == "false")
		useSimulcast = false;

	bool verifySsl = true;
	if (envVerifySsl && std::string(envVerifySsl) == "false")
		verifySsl = false;

	// Set RTC logging severity.
	if (envWebrtcDebug)
	{
		if (std::string(envWebrtcDebug) == "info")
			rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_INFO);
		else if (std::string(envWebrtcDebug) == "warn")
			rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_WARNING);
		else if (std::string(envWebrtcDebug) == "error")
			rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_ERROR);
	}

	auto logLevel = mediasoupclient::Logger::LogLevel::LOG_DEBUG;
	mediasoupclient::Logger::SetLogLevel(logLevel);
	mediasoupclient::Logger::SetDefaultHandler();

	// Initilize mediasoupclient.
	mediasoupclient::Initialize();

	std::cout << "[INFO] welcome to mediasoup broadcaster app!\n" << std::endl;
	int numClient = std::stoi(numberClient);

	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::time_t t                           = std::chrono::system_clock::to_time_t(p);
	std::string time                        = "_" + std::to_string(std::time(&t)) + "_";

	std::cout << "Current Time is: " << std::ctime(&t) << std::endl;
	std::cout << "Number Client is: " << numClient << std::endl;
	std::cout << "Create List Room Start:" << std::endl;
	std::cout << "Run Threads Parallel: " << std::to_string(isRunParallel) << std::endl;
	for (int i = 0; i < numClient; i += 2)
	{
		std::string roomId        = envRoomId + time + std::to_string(i / 2);
		std::string createRoomUrl = envServerUrl;
		createRoomUrl.append("/createRoom/").append(roomId);
		std::cout << "Checking create Room: " << createRoomUrl << std::endl;

		auto checkRoom = cpr::PostAsync(cpr::Url{ createRoomUrl }, cpr::VerifySsl{ verifySsl }).get();
		if (checkRoom.status_code != 200)
		{
			std::cout << "[INFO] Create RoomFail" << checkRoom.text << std::endl;
			return 1;
		}
		std::cout << "Room " << roomId << " is created" << std::endl;
	}
	std::cout << "Create List Room Done" << std::endl;
	std::cout << "Create shared video Source" << std::endl;
	// if (!MediaStreamTrackFactory::factory)
	// 	MediaStreamTrackFactory::createfactory();
	struct webrtc::FrameGeneratorCapturerVideoTrackSource::Config videoSourceConfig;
	videoSourceConfig.width                 = std::stoi(widthSize);
	videoSourceConfig.height                = std::stoi(heightSize);
	videoSourceConfig.num_squares_generated = std::stoi(numberSquare);
	videoSourceConfig.frames_per_second     = 30;
	std::cout << "[INFO] getting frame generator" << std::endl;
	videoSourceConfig.PrintString();
	auto* videoTrackSource = new rtc::RefCountedObject<webrtc::FrameGeneratorCapturerVideoTrackSource>(
	  videoSourceConfig, webrtc::Clock::GetRealTimeClock(), false);
	videoTrackSource->Start();
	for (int i = 0; i < numClient; i++)
	{
		std::string roomId  = envRoomId + time + std::to_string(i / 2);
		std::string baseUrl = envServerUrl;
		baseUrl.append("/rooms/").append(roomId);
		std::cout << "Url Room: " << baseUrl << std::endl;
		std::cout << "Creating Thread to Init Client " << i << std::endl;
		if (isRunParallel)
		{
			std::thread([videoTrackSource, baseUrl, roomId, enableAudio, useSimulcast, verifySsl, i]() {
				std::cout << "Establish Client " << i << std::endl;
				auto r = cpr::GetAsync(cpr::Url{ baseUrl }, cpr::VerifySsl{ verifySsl }).get();
				if (r.status_code != 200)
				{
					std::cerr << "[ERROR] unable to retrieve room info"
					          << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					          << std::endl;
					return;
				}
				std::cout << "Found room, client " << i << " try to join room " << roomId << std::endl;
				Broadcaster* b   = new Broadcaster;
				auto response    = nlohmann::json::parse(r.text);
				std::string name = "Broadcaser_" + std::to_string(i);
				b->Start(baseUrl, enableAudio, useSimulcast, response, videoTrackSource, verifySsl, name);
				LBroadcaster.push_back(b);
				std::cout << "Created Client " << i << std::endl;
			}).detach();
		}
		else
		{
			std::thread([videoTrackSource, baseUrl, roomId, enableAudio, useSimulcast, verifySsl, i]() {
				std::cout << "Establish Client " << i << std::endl;
				auto r = cpr::GetAsync(cpr::Url{ baseUrl }, cpr::VerifySsl{ verifySsl }).get();
				if (r.status_code != 200)
				{
					std::cerr << "[ERROR] unable to retrieve room info"
					          << " [status code:" << r.status_code << ", body:\"" << r.text << "\"]"
					          << std::endl;
					return;
				}
				std::cout << "Found room, client " << i << " try to join room " << roomId << std::endl;
				Broadcaster* b   = new Broadcaster;
				auto response    = nlohmann::json::parse(r.text);
				std::string name = "Broadcaser_" + std::to_string(i);
				b->Start(baseUrl, enableAudio, useSimulcast, response, videoTrackSource, verifySsl, name);
				LBroadcaster.push_back(b);
				std::cout << "Created Client " << i << std::endl;
			}).join();
		}
	}

	std::cout << "[INFO] press Ctrl+C or Cmd+C to leave...(Wait for fully finished create client)"
	          << std::endl;
	while (true)
	{
		std::cout << "Size Broadcaster: " << LBroadcaster.size() << std::endl;
		std::cin.get();
	}
	return 0;
}
