#include <libdnf5/base/base.hpp>
#include <libdnf5/plugin/iplugin.hpp>
#include <libdnf5/utils/bgettext/bgettext-mark-domain.h>
#include <libdnf5/utils/format_locale.hpp>

#include <chrono>
#include <cstring>
#include <thread>

using namespace libdnf5;

namespace {
    constexpr const char *PLUGIN_NAME{"simple"};
    constexpr plugin::Version PLUGIN_VERSION{.major = 0, .minor = 1, .micro = 0};
    constexpr PluginAPIVersion REQUIRED_PLUGIN_API_VERSION{.major = 2, .minor = 0};

    constexpr const char *attrs[]{"author.name", "author.email", "description", nullptr};
    constexpr const char *attrs_value[]{
        "Jiri Hnidek",
        "jhnidek@redhat.com",
        "A simple example libdnf5 plugin that tries to print progress messages."
    };

    /// Class for displaying simple messages without any argument.
    class SimpleMessage : public Message {
    public:
        SimpleMessage() : text() {}
        explicit SimpleMessage(const BgettextMessage text) : text(text) {}

        std::string format(const bool translate, const utils::Locale * locale) const override {
            return utils::format(locale, translate, text, 1);
        }

    private:
        BgettextMessage text;
    };

    /// Class for displaying simple messages with an argument.
    class ParamMessage : public Message {
    public:
        ParamMessage() : text(), param() {}
        explicit ParamMessage(const BgettextMessage text, std::string param) : text(text), param(std::move(param)) {}

        std::string format(const bool translate, const utils::Locale * locale) const override {
            return utils::format(locale, translate, text, 1, param);
        }

    private:
        BgettextMessage text;
        std::string param;
    };

    class SimplePlugin final : public plugin::IPlugin {
    public:
        /// Use the default IPlugin constructor; we only need the Base object.
        using IPlugin::IPlugin;

        /// Fill in the API version of your plugin.
        /// This is used to check if the provided plugin API version is compatible with the library's plugin API version.
        /// MANDATORY to override.
        [[nodiscard]] PluginAPIVersion get_api_version() const noexcept override { return REQUIRED_PLUGIN_API_VERSION; }

        /// Enter the name of your new plugin.
        /// This is used in log messages when an action or error related to the plugin occurs.
        /// MANDATORY to override.
        [[nodiscard]] const char *get_name() const noexcept override { return PLUGIN_NAME; }

        /// Fill in the version of your plugin.
        /// This is used in informative and debugging log messages.
        /// MANDATORY to override.
        [[nodiscard]] plugin::Version get_version() const noexcept override { return PLUGIN_VERSION; }

        /// Add custom attributes, such as information about yourself and a description of the plugin.
        /// These can be used to query plugin-specific data through the API.
        /// Optional to override.
        [[nodiscard]] const char *const *get_attributes() const noexcept override { return attrs; }

        const char *get_attribute(const char *attribute) const noexcept override {
            for (size_t i = 0; attrs[i]; ++i) {
                if (std::strcmp(attribute, attrs[i]) == 0) {
                    return attrs_value[i];
                }
            }
            return nullptr;
        }

        /// This hook runs after the Base object has been set up.
        /// Report an example progress counting from 0 to 10.
        void post_base_setup() override {
            report_progress();
        }

    private:
        /// Example of reporting progress via the libdnf5 progress API.
        /// It starts a new progress entry, counts from 0 to 10 (reporting the
        /// current value on each step), and finally marks the progress as done.
        void report_progress() const {
            constexpr int64_t total = 10;

            // Start a new progress entry. The returned handle identifies this
            // progress for the subsequent UPDATE and END_OK calls.
            SimpleMessage start_msg(M_("Counting"));
            const int handle = get_base().progress(
                0,
                base::InteractionCallbacks::ProgressState::NEW,
                &start_msg, 0, total);

            // Report each step of the counter, including the current value in
            // the message, and wait 200 milliseconds between increments so the
            // progress is noticeable.
            for (int64_t count = 0; count <= total; ++count) {
	        auto progress_msg = ParamMessage(M_("Progress: {}"), std::to_string(count));
                get_base().progress(
                    handle,
                    base::InteractionCallbacks::ProgressState::UPDATE,
                    &progress_msg, count, total);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

            // Mark the progress as successfully finished.
	    auto end_msg = ParamMessage(M_("Counting finished {} insisible counts"), std::to_string(total));
            get_base().progress(
                handle,
                base::InteractionCallbacks::ProgressState::END_OK,
                &end_msg, total, total);
        }
    };
} // namespace


/// Below is a block of functions with C linkage used for loading the plugin binaries from disk.
/// All of these are MANDATORY to implement.

/// Return plugin's API version.
PluginAPIVersion libdnf_plugin_get_api_version(void) {
    return REQUIRED_PLUGIN_API_VERSION;
}

/// Return plugin's name.
const char *libdnf_plugin_get_name(void) {
    return PLUGIN_NAME;
}

/// Return plugin's version.
plugin::Version libdnf_plugin_get_version(void) {
    return PLUGIN_VERSION;
}

/// Return the instance of the implemented plugin.
plugin::IPlugin *libdnf_plugin_new_instance(
    [[maybe_unused]] LibraryVersion library_version,
    plugin::IPluginData &data,
    [[maybe_unused]] ConfigParser &parser) try {
    return new SimplePlugin(data);
} catch (...) {
    return nullptr;
}

/// Delete the plugin instance.
void libdnf_plugin_delete_instance(plugin::IPlugin *plugin_object) {
    delete plugin_object;
}

