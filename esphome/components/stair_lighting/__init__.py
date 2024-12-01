import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.light.types import AddressableLightEffect
from esphome.components.light.effects import register_addressable_effect
from esphome.const import (
    CONF_ID,
    CONF_NAME,
)

CONF_STAIR_LIGHTING_ID = "stair_lighting_id"

stair_lighting_ns = cg.esphome_ns.namespace("stair_lighting")

StairLightingComponent = stair_lighting_ns.class_("StairLightingComponent", cg.Component)
StairLightingStepConfig = stair_lighting_ns.struct("StairLightingStepConfig")
StairLightingEffect = stair_lighting_ns.class_("StairLightingEffect", AddressableLightEffect)
ColorStairLightingEffect = stair_lighting_ns.class_("ColorStairLightingEffect", StairLightingEffect)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(StairLightingComponent),
        cv.Required("steps"): cv.ensure_list(
            cv.Schema(
                {
                    cv.Required("size"): cv.positive_int,
                    cv.Optional("reversed", default="false"): cv.boolean
                }
            )
        ),
    }
)


async def to_code(config):
    component = cg.new_Pvariable(config[CONF_ID])
    steps = []
    offset = 0
    for step_config in config.get("steps", []):
        step = cg.StructInitializer(
            StairLightingStepConfig,
            ("offset", offset),
            ("size", step_config["size"]),
            ("reversed", step_config["reversed"])
        )
        steps.append(step)
        offset += step_config["size"]
    cg.add(component.set_steps_config(steps))
    await cg.register_component(component, config)


@register_addressable_effect(
    "color_stair_lighting",
    ColorStairLightingEffect,
    "Color Stair Lighting",
    {
        cv.GenerateID(CONF_STAIR_LIGHTING_ID): cv.use_id(StairLightingComponent),
        cv.Optional("next_step_interval", default="350ms"): cv.positive_time_period_milliseconds,
        cv.Optional("progress_step_interval", default="1s"): cv.positive_time_period_milliseconds,
    },
)
async def color_stair_lighting_effect_to_code(config, effect_id):
    parent = await cg.get_variable(config[CONF_STAIR_LIGHTING_ID])
    effect = cg.new_Pvariable(effect_id, config[CONF_NAME])
    cg.add(effect.set_parent(parent))
    cg.add(effect.set_next_step_interval(config["next_step_interval"]))
    cg.add(effect.set_progress_step_interval(config["progress_step_interval"]))
    return effect
