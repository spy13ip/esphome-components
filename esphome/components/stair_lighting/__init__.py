import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components.light.types import AddressableLightEffect
from esphome.components.light.effects import register_addressable_effect
from esphome.const import (
    CONF_ID,
    CONF_NAME,
)

AUTO_LOAD = ["binary_sensor"]

CONF_STAIR_LIGHTING_ID = "stair_lighting_id"
CONF_EFFECT_BRIGHTNESS = "effect_brightness"
CONF_NIGHT_BRIGHTNESS = "night_brightness"

stair_lighting_ns = cg.esphome_ns.namespace("stair_lighting")
# types
StairLightingComponent = stair_lighting_ns.class_("StairLightingComponent", cg.Component)
StairLightingStepConfig = stair_lighting_ns.struct("StairLightingStepConfig")
StairLightingEffect = stair_lighting_ns.class_("StairLightingEffect", AddressableLightEffect)
ColorStairLightingEffect = stair_lighting_ns.class_("ColorStairLightingEffect", StairLightingEffect)
# automation
ControlAction = stair_lighting_ns.class_("StairLightingControlAction", automation.Action)
TurnUpAction = stair_lighting_ns.class_("StairLightingTurnUpAction", automation.Action)
TurnDownAction = stair_lighting_ns.class_("StairLightingTurnDownAction", automation.Action)
TurnFullAction = stair_lighting_ns.class_("StairLightingTurnFullAction", automation.Action)
TurnOffAction = stair_lighting_ns.class_("StairLightingTurnOffAction", automation.Action)

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
        cv.Optional(CONF_EFFECT_BRIGHTNESS): cv.percentage,
        cv.Optional(CONF_NIGHT_BRIGHTNESS): cv.percentage,
    }
).extend(cv.COMPONENT_SCHEMA)


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
    cg.add(component.set_effect_brightness(config[CONF_EFFECT_BRIGHTNESS]))
    cg.add(component.set_night_brightness(config[CONF_NIGHT_BRIGHTNESS]))
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


STAIR_LIGHTING_ACTION_SCHEMA = automation.maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(StairLightingComponent),
    }
)


@automation.register_action(
    "stair_lighting.control",
    ControlAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(StairLightingComponent),
            cv.Optional(CONF_EFFECT_BRIGHTNESS): cv.templatable(cv.percentage),
            cv.Optional(CONF_NIGHT_BRIGHTNESS): cv.templatable(cv.percentage),
        }
    ),
)
async def stair_lighting_control_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    if CONF_EFFECT_BRIGHTNESS in config:
        template = await cg.templatable(config[CONF_EFFECT_BRIGHTNESS], args, float)
        cg.add(var.set_effect_brightness(template))
    if CONF_NIGHT_BRIGHTNESS in config:
        template = await cg.templatable(config[CONF_NIGHT_BRIGHTNESS], args, float)
        cg.add(var.set_night_brightness(template))
    return var


@automation.register_action("stair_lighting.turn_up", TurnUpAction, STAIR_LIGHTING_ACTION_SCHEMA)
@automation.register_action("stair_lighting.turn_down", TurnDownAction, STAIR_LIGHTING_ACTION_SCHEMA)
@automation.register_action("stair_lighting.turn_full", TurnFullAction, STAIR_LIGHTING_ACTION_SCHEMA)
@automation.register_action("stair_lighting.turn_off", TurnOffAction, STAIR_LIGHTING_ACTION_SCHEMA)
async def stair_lighting_turn_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    action = cg.new_Pvariable(action_id, template_arg, parent)
    return action
