#include <basalt/api/gfx/backend/ext/effect.h>

#include <basalt/gfx/backend/command_list_p.h>
#include <basalt/gfx/backend/commands.h>

namespace basalt::gfx::ext {

auto EffectCommandEncoder::begin_effect(CommandList& cmdList,
                                        EffectId const effectId) -> void {
  CommandListP::add<CommandBeginEffect>(cmdList, effectId);
}

auto EffectCommandEncoder::end_effect(CommandList& cmdList) -> void {
  CommandListP::add<CommandEndEffect>(cmdList);
}

auto EffectCommandEncoder::begin_effect_pass(CommandList& cmdList,
                                             u32 const passIndex) -> void {
  CommandListP::add<CommandBeginEffectPass>(cmdList, passIndex);
}

auto EffectCommandEncoder::end_effect_pass(CommandList& cmdList) -> void {
  CommandListP::add<CommandEndEffectPass>(cmdList);
}

} // namespace basalt::gfx::ext
