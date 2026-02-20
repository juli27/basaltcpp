#include <basalt/api/gfx/backend/ext/effect.h>

#include <basalt/gfx/backend/command_list_p.h>
#include <basalt/gfx/backend/commands.h>

namespace basalt::gfx::ext {

auto EffectCommandEncoder::begin_effect(CommandList& cmdList,
                                        EffectId const effectId) -> void {
  cmdList.add<CommandBeginEffect>(effectId);
}

auto EffectCommandEncoder::end_effect(CommandList& cmdList) -> void {
  cmdList.add<CommandEndEffect>();
}

auto EffectCommandEncoder::begin_effect_pass(CommandList& cmdList,
                                             u32 const passIndex) -> void {
  cmdList.add<CommandBeginEffectPass>(passIndex);
}

auto EffectCommandEncoder::end_effect_pass(CommandList& cmdList) -> void {
  cmdList.add<CommandEndEffectPass>();
}

} // namespace basalt::gfx::ext
