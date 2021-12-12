#include "SurvivalBoundArrowWeightFix.hpp"

#include <cstring>

#include <xbyak/xbyak.h>

#include <SKSE/Interfaces.h>
#include <RE/T/TESForm.h>

#include "global.hpp"
#include "expectedbytes.hpp"
#include "offsets.hpp"
#include "config/WBAConfig.hpp"
#include "utilities/printerror.hpp"

namespace {
    bool _isWeightlessArrow(RE::TESBoundObject* const form)
    {
        return WBAConfig::getInstance().isWeightlessArrow(form);
    }

    void _handleMessage(SKSE::MessagingInterface::Message* const message)
    {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            auto dataHandler = RE::TESDataHandler::GetSingleton();

            auto& config = WBAConfig::getInstance();
            config.loadGameForms(dataHandler);
        }
    }

    struct _Patch : Xbyak::CodeGenerator {
        explicit _Patch()
        {
            using re::TESBoundObject::GetWeight;
            using namespace re::fix::survivalboundarrowweight;

            Xbyak::Label isWeightlessArrowLabel;
            Xbyak::Label continueOverwrittenLabel;
            Xbyak::Label continueLabel;
            Xbyak::Label exitLabel;

            mov(rcx, rbx);
            call(ptr[rip + isWeightlessArrowLabel]);

            test(rax, rax);
            jz(continueOverwrittenLabel);
            jmp(ptr[rip + exitLabel]);

            L(continueOverwrittenLabel);
#if defined(SKYRIM_VERSION_SE)
            lea(rdi, ptr[rbx + 0x0b0]);
#elif defined(SKYRIM_VERSION_AE)
            mov(eax, 0x0b0);
#else
#    error "Invalid Skyrim version."
#endif
            jmp(ptr[rip + continueLabel]);

            L(isWeightlessArrowLabel);
            dq(reinterpret_cast<std::uint64_t>(_isWeightlessArrow));

            L(continueLabel);
            dq(GetWeight.address() + continueOffset);

            L(exitLabel);
            dq(GetWeight.address() + exitOffset);
        }
    };

    bool _isSurvivalBoundArrowWeightPatchable()
    {
        using namespace re::fix::survivalboundarrowweight;
        using re::TESBoundObject::GetWeight;

        if (std::memcmp(
                reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(
                    GetWeight.address() + patchOffset)),
                expectedPatchBytes,
                sizeof expectedPatchBytes) != 0) {
            LOG_CRITICAL(
                "[SURVIVALBOUNDARROWWEIGHT] Expected bytes for soul trap handling at patch offset not found."sv);
            return false;
        }

        if (std::memcmp(
                reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(
                    GetWeight.address() + continueOffset)),
                expectedContinueBytes,
                sizeof expectedContinueBytes) != 0) {
            LOG_CRITICAL(
                "[SURVIVALBOUNDARROWWEIGHT] Expected bytes for soul trap handling at continue offset not found."sv);
            return false;
        }

        if (std::memcmp(
                reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(
                    GetWeight.address() + exitOffset)),
                expectedExitBytes,
                sizeof expectedExitBytes) != 0) {
            LOG_CRITICAL(
                "[SURVIVALBOUNDARROWWEIGHT] Expected bytes for soul trap handling at exit offset not found."sv);
            return false;
        }

        return true;
    }
} // namespace

bool installSurvivalBoundArrowWeightFix()
{
    using re::TESBoundObject::GetWeight;
    using namespace re::fix::survivalboundarrowweight;

    try {
        auto& config = WBAConfig::getInstance();
        config.loadConfigsFromFile();

        auto messaging = SKSE::GetMessagingInterface();
        messaging->RegisterListener(_handleMessage);
    } catch (const std::exception& error) {
        printError(error);
        LOG_ERROR("Not installing survivalBoundArrowWeight patch.");

        return false;
    }

    if (!_isSurvivalBoundArrowWeightPatchable()) {
        // More detailed logs should already be printed by the checker so no
        // need to do anything here.
        return false;
    }

    _Patch patch;
    patch.ready();

    LOG_INFO_FMT("[SURVIVALBOUNDARROWWEIGHT] Patch size: {}", patch.getSize());

    auto& trampoline = SKSE::GetTrampoline();
    SKSE::AllocTrampoline(80);
    trampoline.write_branch<5>(
        GetWeight.address() + patchOffset,
        trampoline.allocate(patch));

    return true;
}
