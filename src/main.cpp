#include <memory>

#include <SKSE/SKSE.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "global.hpp"
#include "version.hpp"
#include "SurvivalBoundArrowWeightFix.hpp"

bool setUpLogging()
{
    using namespace std::literals;
    namespace logger = SKSE::log;

    auto path = logger::log_directory();
    if (!path.has_value()) {
        LOG_ERROR("Could not open log directory.");
        return false;
    }

    *path /= version::PROJECT;
    *path += ".log"sv;
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        path->string(),
        true);
    auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
    log->set_level(spdlog::level::trace);
    log->flush_on(spdlog::level::trace);
#else
    log->set_level(spdlog::level::info);
    log->flush_on(spdlog::level::info);
#endif

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

    LOG_INFO_FMT("{} v{}"sv, version::PROJECT, version::NAME);

    return true;
}

//#include "versiondb.hpp"

//bool DumpOffsets() {
//	VersionDb db;
//
//	if (!db.Load()) {
//		LOG_CRITICAL("Failed to load offset database."sv);
//		return false;
//	}
//
//	const std::string& version{db.GetLoadedVersionString()};
//
//	db.Dump("offsets-" + version + ".txt");
//	LOG_INFO_FMT("Dumped offsets for {}", version);
//
//	return true;
//}

#if defined(SKYRIM_VERSION_SE)
extern "C" DLLEXPORT bool SKSEAPI
    SKSEPlugin_Query(const SKSE::QueryInterface* skse, SKSE::PluginInfo* info)
{
    setUpLogging();

    info->infoVersion = SKSE::PluginInfo::kVersion;
    info->name = version::PROJECT.data();
    info->version = version::MAJOR;

    if (skse->IsEditor()) {
        LOG_CRITICAL("Loaded in editor, marking as incompatible"sv);
        return false;
    }

    const auto ver = skse->RuntimeVersion();
    if (ver < SKSE::RUNTIME_1_5_39) {
        LOG_CRITICAL_FMT("Unsupported runtime version {}"sv, ver.string());
        return false;
    }

    return true;
}

extern "C" DLLEXPORT bool SKSEAPI
    SKSEPlugin_Load(const SKSE::LoadInterface* skse)
{
    LOG_INFO_FMT("Loaded {} v{}", version::PROJECT, version::NAME);   

    SKSE::Init(skse);

    return installSurvivalBoundArrowWeightFix();
}
#elif defined(SKYRIM_VERSION_AE)
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v;

    v.PluginVersion(
        REL::Version(version::MAJOR, version::MINOR, version::PATCH));
    v.PluginName(version::PROJECT);
    v.AuthorName("Seally");
    v.UsesAddressLibrary(true);
    v.UsesSigScanning(false);
    v.CompatibleVersions({SKSE::RUNTIME_1_6_318});

    return v;
}();

extern "C" DLLEXPORT bool SKSEPlugin_Load(const SKSE::LoadInterface* skse)
{
    setUpLogging();

    LOG_INFO_FMT("Loaded {} v{}", version::PROJECT, version::NAME);
    SKSE::Init(skse);

    return installSurvivalBoundArrowWeightFix();
}
#else
#    error "SKYRIM_VERSION_<version> is not defined."
#endif
