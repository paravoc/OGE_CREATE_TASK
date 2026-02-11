// problem_factory.h
#pragma once

#include "problem_base.h"
#include <vector>
#include <memory>
 
namespace OGE {

    class ProblemFactory {
    public:
        static std::unique_ptr<ProblemBase> create_problem(
            const std::string& type_id,
            const GenerationConfig& config = {});

        static std::unique_ptr<ProblemBase> from_json(const std::string& json);

        static std::vector<std::unique_ptr<ProblemBase>> create_problems(
            const std::string& type_id,
            int count,
            const GenerationConfig& base_config = {});

        static std::vector<std::unique_ptr<ProblemBase>> create_problems(
            const std::vector<std::string>& type_ids,
            const GenerationConfig& base_config = {});

        static bool validate_config(const std::string& type_id,
            const GenerationConfig& config);
    };

} // namespace OGE