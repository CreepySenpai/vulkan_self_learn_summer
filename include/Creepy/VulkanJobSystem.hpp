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


    class VulkanCommandBufferSubmitData{
        public:
            constexpr void AddToSubmit(std::function<void(const vk::CommandBuffer)>&& submit){
                m_submits.emplace_back(std::forward<std::function<void(const vk::CommandBuffer)>&&>(submit));
            }

            constexpr size_t GetCount() noexcept{
                return m_submits.size();
            }

            constexpr void Submit(uint32_t index, const vk::CommandBuffer cmd){
                m_submits.at(index)(cmd);
            }

        private:
            std::vector<std::function<void(const vk::CommandBuffer)>> m_submits;
    };

}