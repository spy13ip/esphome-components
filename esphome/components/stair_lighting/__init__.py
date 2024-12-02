import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components.light.types import AddressableLightEffect
from esphome.components.light.effects import register_addressable_effect
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_SIZE,
    CONF_REVERSED,
)

AUTO_LOAD = ["binary_sensor"]

CONF_STAIR_LIGHTING_ID = "stair_lighting_id"
CONF_EFFECT_BRIGHTNESS = "effect_brightness"
CONF_LOWER_SENSOR = "sensor_lower"
CONF_UPPER_SENSOR = "sensor_upper"
CONF_PASSAGE_TIMEOUT = "passage_timeout"
CONF_OFF_TIMEOUT = "off_timeout"
CONF_NIGHT_BRIGHTNESS = "night_brightness"
CONF_NEXT_STEP_INTERVAL = "next_step_interval"
CONF_PROGRESS_STEP_INTERVAL = "progress_step_interval"
CONF_STEPS = "steps"

binary_sensor_ns = cg.esphome_ns.namespace("binary_sensor")
BinarySensor = binary_sensor_ns.class_("BinarySensor", cg.EntityBase)

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
        cv.Required(CONF_STEPS): cv.ensure_list(
            cv.Schema(
                {
                    cv.Required(CONF_SIZE): cv.positive_int,
                    cv.Optional(CONF_REVERSED, default="false"): cv.boolean
                }
            )
        ),
        cv.Required(CONF_LOWER_SENSOR): cv.use_id(BinarySensor),
        cv.Required(CONF_UPPER_SENSOR): cv.use_id(BinarySensor),
        cv.Optional(CONF_PASSAGE_TIMEOUT, default="5s"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_OFF_TIMEOUT, default="20s"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_EFFECT_BRIGHTNESS, default="100%"): cv.percentage,
        cv.Optional(CONF_NIGHT_BRIGHTNESS, default="20%"): cv.percentage,
    }
).extend(cv.COMPONENT_SCHEMA)

SIMPLE_ACTION_SCHEMA = automation.maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(StairLightingComponent),
    }
)

CONTROL_ACTION_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(StairLightingComponent),
        cv.Optional(CONF_EFFECT_BRIGHTNESS): cv.templatable(cv.percentage),
        cv.Optional(CONF_NIGHT_BRIGHTNESS): cv.templatable(cv.percentage),
    }
)


async def to_code(config):
    component = cg.new_Pvariable(config[CONF_ID])
    steps = []
    offset = 0
    for step_config in config.get(CONF_STEPS, []):
        step = cg.StructInitializer(
            StairLightingStepConfig,
            ("offset", offset),
            ("size", step_config[CONF_SIZE]),
            ("reversed", step_config[CONF_REVERSED])
        )
        steps.append(step)
        offset += step_config[CONF_SIZE]
    cg.add(component.set_steps_config(steps))
    lower_sensor = await cg.get_variable(config[CONF_LOWER_SENSOR])
    upper_sensor = await cg.get_variable(config[CONF_UPPER_SENSOR])
    cg.add(component.set_lower_sensor(lower_sensor))
    cg.add(component.set_upper_sensor(upper_sensor))
    cg.add(component.set_passage_timeout(config[CONF_PASSAGE_TIMEOUT]))
    cg.add(component.set_off_timeout(config[CONF_OFF_TIMEOUT]))
    cg.add(component.set_effect_brightness(config[CONF_EFFECT_BRIGHTNESS]))
    cg.add(component.set_night_brightness(config[CONF_NIGHT_BRIGHTNESS]))
    await cg.register_component(component, config)


@register_addressable_effect(
    "color_stair_lighting",
    ColorStairLightingEffect,
    "Color Stair Lighting",
    {
        cv.GenerateID(CONF_STAIR_LIGHTING_ID): cv.use_id(StairLightingComponent),
        cv.Optional(CONF_NEXT_STEP_INTERVAL, default="350ms"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_PROGRESS_STEP_INTERVAL, default="1s"): cv.positive_time_period_milliseconds,
    },
)
async def color_stair_lighting_effect_to_code(config, effect_id):
    parent = await cg.get_variable(config[CONF_STAIR_LIGHTING_ID])
    effect = cg.new_Pvariable(effect_id, config[CONF_NAME])
    cg.add(effect.set_parent(parent))
    cg.add(effect.set_next_step_interval(config[CONF_NEXT_STEP_INTERVAL]))
    cg.add(effect.set_progress_step_interval(config[CONF_PROGRESS_STEP_INTERVAL]))
    return effect


@automation.register_action("stair_lighting.control", ControlAction, CONTROL_ACTION_SCHEMA)
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


@automation.register_action("stair_lighting.turn_up", TurnUpAction, SIMPLE_ACTION_SCHEMA)
@automation.register_action("stair_lighting.turn_down", TurnDownAction, SIMPLE_ACTION_SCHEMA)
@automation.register_action("stair_lighting.turn_full", TurnFullAction, SIMPLE_ACTION_SCHEMA)
@automation.register_action("stair_lighting.turn_off", TurnOffAction, SIMPLE_ACTION_SCHEMA)
async def stair_lighting_turn_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    action = cg.new_Pvariable(action_id, template_arg, parent)
    return action
