import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.light.types import AddressableLightEffect
from esphome.components.light.effects import register_addressable_effect
from esphome.const import (
    CONF_NAME,
    CONF_UPDATE_INTERVAL,
)

stair_lighting_ns = cg.esphome_ns.namespace("stair_lighting")
StepConfig = stair_lighting_ns.struct("StepConfig")
StairLightingEffect = stair_lighting_ns.class_("StairLightingEffect", AddressableLightEffect)
ColorStairLightingEffect = stair_lighting_ns.class_("ColorStairLightingEffect", StairLightingEffect)


@register_addressable_effect(
    "color_stair_lighting",
    ColorStairLightingEffect,
    "Color Stair Lighting",
    {
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
    steps = []
    for step_config in config.get("steps", []):
        steps.append(
            cg.StructInitializer(
                StepConfig,
                ("size", step_config["size"]),
                ("reversed", step_config["reversed"]),
            )
        )
    var = cg.new_Pvariable(effect_id, config[CONF_NAME], steps)
    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))
    cg.add(var.set_next_step_interval(config["next_step_interval"]))
    cg.add(var.set_progress_step_interval(config["progress_step_interval"]))
    return var
