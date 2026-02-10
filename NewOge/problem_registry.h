// problem_registry.h
#pragma once

#include "problem_base.h"
#include <functional>
#include <memory>

namespace OGE {

    class ProblemRegistry {
    private:
        static std::unordered_map<std::string, ProblemMeta>& get_registry();
        static std::unordered_map<std::string,
            std::function<std::unique_ptr<ProblemBase>()>>&get_factory_registry();

    public:
        static void register_type(const ProblemMeta& meta,
            std::function<std::unique_ptr<ProblemBase>()> factory);

        static bool has_type(const std::string& type_id);
        static const ProblemMeta& get_meta(const std::string& type_id);
        static std::vector<std::string> get_all_types();
        static std::vector<std::string> get_types_by_problem_number(int problem_number);
        static std::vector<std::string> get_types_by_tag(const std::string& tag);
        static std::unique_ptr<ProblemBase> create_instance(const std::string& type_id);
    };

} // namespace OGE