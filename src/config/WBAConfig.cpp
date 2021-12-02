#include "WBAConfig.hpp"

#include <filesystem>

#include <toml++/toml.h>

#include <RE/A/Actor.h>
#include <RE/B/BGSDefaultObjectManager.h>
#include <RE/T/TESDataHandler.h>
#include <RE/T/TESGlobal.h>
#include <RE/T/TESSoulGem.h>
#include <SKSE/SKSE.h>

#include "../global.hpp"
#include "FormError.hpp"
#include "ParseError.hpp"
#include "../formatters/TESBoundObject.hpp"
#include "../utilities/printerror.hpp"

using namespace std::literals;

void WBAConfig::_readIndividualConfigs()
{
    std::vector<std::filesystem::path> configPaths;

    for (const auto& entry : std::filesystem::directory_iterator("Data/"sv)) {
        if (entry.exists() && !entry.path().empty() &&
            entry.path().extension() == ".toml"sv) {
            const auto fileName = entry.path().filename();
            const auto fileNameStr = fileName.string();

            if (fileNameStr.starts_with("WBA_"sv)) {
                LOG_INFO_FMT("Found WBA configuration file: {}"sv, fileNameStr);
                configPaths.push_back(entry.path());
            }
        }
    }

    if (configPaths.empty()) {
        throw WBAConfigLoadError("No WBA configuration files found.");
    }

    std::size_t validConfigsCount = 0;

    for (const auto& configPath : configPaths) {
        toml::table table;

        std::string configPathStr = configPath.string();

        try {
            table = toml::parse_file(configPathStr);

            LOG_INFO_FMT(
                "Reading individual configuration file: {}"sv,
                configPathStr);

            validConfigsCount += _readAndCountConfigs(table);
        } catch (const toml::parse_error& error) {
            LOG_WARN_FMT(
                "Error while parsing individual configuration file \"{}\": {}"sv,
                configPathStr,
                error.what());
        }
    }

#ifndef NDEBUG
    // Print the loaded configuration (we can't read the in-game forms yet.
    // Game hasn't fully initialized.)
    LOG_TRACE("Loaded weightless arrows from TOML:"sv);

    for (const auto& arrow : _weightlessArrows) {
        LOG_TRACE_FMT("- {}"sv, arrow);
    }
#endif // NDEBUG

    if (validConfigsCount <= 0) {
        throw WBAConfigLoadError("No valid configurations found.");
    }
}

std::size_t WBAConfig::_readAndCountConfigs(const toml::table& table)
{
    std::size_t validConfigsCount = 0;

    if (const auto arrowFormIds = table["weightlessArrows"sv].as_array();
        arrowFormIds != nullptr) {
        std::size_t index = 0;
        for (const toml::node& elem : *arrowFormIds) {
            try {
                elem.visit([&, this](auto&& el) {
                    if constexpr (toml::is_array<decltype(el)>) {
                        _weightlessArrows.emplace(el);
                        ++validConfigsCount;
                    } else {
                        throw EntryError(
                            index,
                            fmt::format(
                                FMT_STRING(
                                    "weightlessArrows[{}] is not an array"),
                                index));
                    }
                });
            } catch (const std::exception& error) {
                printError(error, 1);
            }
            ++index;
        }
    }

    return validConfigsCount;
}

void WBAConfig::readConfigs() { _readIndividualConfigs(); }

void WBAConfig::loadGameForms(RE::TESDataHandler* const dataHandler)
{
    _loadWeightlessArrowForms(dataHandler);
}

void _checkFormIsNotNull(RE::TESForm* form, const FormId& formId)
{
    if (form == nullptr) {
        throw MissingFormError(formId);
    }
}

void _checkFormIsAmmo(RE::TESForm* form)
{
    if (!form->IsAmmo()) {
        throw UnexpectedFormTypeError(
            RE::FormType::Ammo,
            form->GetFormType(),
            form->GetName());
    }
}

void WBAConfig::_loadWeightlessArrowForms(RE::TESDataHandler* const dataHandler)
{
    for (const auto& formId : _weightlessArrows) {
        try {
            const auto form =
                dataHandler->LookupForm(formId.id(), formId.pluginName());

            _checkFormIsNotNull(form, formId);
            _checkFormIsAmmo(form);

            _weightlessArrowForms.emplace(form->As<RE::TESBoundObject>());
        } catch (const std::exception& error) {
            printError(error);
        }
    }

    LOG_INFO("Listing weightless bound arrows:");
    for (const auto& arrow : _weightlessArrowForms) {
        LOG_INFO_FMT("- {}", *arrow);
    }
}

WBAConfigLoadError::WBAConfigLoadError(const std::string& message)
    : std::runtime_error{message}
{}
