import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.light.types import AddressableLightEffect
from esphome.components.light.effects import register_addressable_effect
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_UPDATE_INTERVAL,
)

CONF_STAIR_LIGHTING_ID = "stair_lighting_id"

stair_lighting_ns = cg.esphome_ns.namespace("stair_lighting")

StairLightingComponent = stair_lighting_ns.class_("StairLightingComponent", cg.Component)

StepConfig = stair_lighting_ns.struct("StepConfig")
StairLightingEffect = stair_lighting_ns.class_("StairLightingEffect", AddressableLightEffect)
ColorStairLightingEffect = stair_lighting_ns.class_("ColorStairLightingEffect", StairLightingEffect)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(StairLightingComponent),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)


@register_addressable_effect(
    "color_stair_lighting",
    ColorStairLightingEffect,
    "Color Stair Lighting",
    {
        cv.GenerateID(CONF_STAIR_LIGHTING_ID): cv.use_id(StairLightingComponent),
        cv.Required("steps"): cv.ensure_list(
            cv.Schema(
                {
                    cv.Required("size"): cv.positive_int,
                    cv.Optional("reversed", default="false"): cv.boolean
                }
            )
        ),
        cv.Optional(CONF_UPDATE_INTERVAL, default="32ms"): cv.positive_time_period_milliseconds,
        cv.Optional("next_step_interval", default="350ms"): cv.positive_time_period_milliseconds,
        cv.Optional("progress_step_interval", default="1s"): cv.positive_time_period_milliseconds,
    },
)
async def color_stair_lighting_effect_to_code(config, effect_id):
    parent = await cg.get_variable(config[CONF_STAIR_LIGHTING_ID])
    effect = cg.new_Pvariable(effect_id, config[CONF_NAME])
    cg.add(effect.set_stair_lighting(parent))
    steps = []
    for step_config in config.get("steps", []):
        steps.append(
            cg.StructInitializer(
                StepConfig,
                ("size", step_config["size"]),
                ("reversed", step_config["reversed"]),
            )
        )
    cg.add(effect.add_steps(steps))
    cg.add(effect.set_update_interval(config[CONF_UPDATE_INTERVAL]))
    cg.add(effect.set_next_step_interval(config["next_step_interval"]))
    cg.add(effect.set_progress_step_interval(config["progress_step_interval"]))
    return effect
