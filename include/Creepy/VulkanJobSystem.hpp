#pragma once

#include <functional>
#include <vector>
#include <ranges>

namespace Creepy {

    class VulkanJobSystem{
        public:
            constexpr void AddJob(std::function<void()>&& job) noexcept {
                m_jobs.emplace_back(std::forward<std::function<void()>&&>(job));
            }

            // Excute Job From Top -> Bottom
            constexpr void Execute() noexcept {
                for(auto&& job : m_jobs | std::views::reverse){
                    job();
                }
            }

        private:
            std::vector<std::function<void()>> m_jobs;
    };

}