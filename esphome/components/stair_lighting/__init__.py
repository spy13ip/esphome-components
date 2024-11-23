from esphome import automation
import esphome.codegen as cg
from esphome.components import light
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESSABLE_LIGHT_ID, CONF_ID

AUTO_LOAD = ["sensor", "binary_sensor", "switch"]

# Config constants
CONF_STEPS = "steps"
CONF_START = "start"
CONF_END = "end"
CONF_EFFECT_BRIGHTNESS = "effect_brightness"
CONF_NIGHT_BRIGHTNESS = "night_brightness"

# Classes
stair_lighting_ns = cg.esphome_ns.namespace("stair_lighting")

StairLightingComponent = stair_lighting_ns.class_(
    "StairLightingComponent", cg.PollingComponent
)
StairLightingStep = stair_lighting_ns.class_("StairLightingStep")
StairLightingEffect = stair_lighting_ns.class_("StairlightingEffect")
StairLightingTurnOnAction = stair_lighting_ns.class_(
    "StairLightingTurnOnAction", automation.Action
)
StairLightingTurnOffAction = stair_lighting_ns.class_(
    "StairLightingTurnOffAction", automation.Action
)

STEP_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ADDRESSABLE_LIGHT_ID): cv.use_id(light.AddressableLightState),
    }
)


async def step_to_code(config):
    light = await cg.get_variable(config[CONF_ADDRESSABLE_LIGHT_ID])
    return light


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(StairLightingComponent),
        cv.Required(CONF_STEPS): cv.All(cv.ensure_list(STEP_SCHEMA), cv.Length(min=1)),
        cv.Optional(CONF_EFFECT_BRIGHTNESS, default="100%"): cv.percentage,
        cv.Optional(CONF_NIGHT_BRIGHTNESS, default="10%"): cv.percentage,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    steps = []
    for step_config in config.get(CONF_STEPS, []):
        steps.append(await step_to_code(step_config))
    cg.add(var.add_steps(steps))
    cg.add(var.set_effect_brightness(config[CONF_EFFECT_BRIGHTNESS]))
    cg.add(var.set_night_brightness(config[CONF_NIGHT_BRIGHTNESS]))

    await cg.register_component(var, config)


@automation.register_action(
    "stair_lighting.turn_on",
    StairLightingTurnOnAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(StairLightingComponent),
            cv.Optional(CONF_EFFECT_BRIGHTNESS): cv.templatable(cv.percentage),
            cv.Optional(CONF_NIGHT_BRIGHTNESS): cv.templatable(cv.percentage),
        }
    ),
)
async def stair_lighting_turn_on_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    if CONF_EFFECT_BRIGHTNESS in config:
        templ = await cg.templatable(config[CONF_EFFECT_BRIGHTNESS], args, float)
        cg.add(var.set_effect_brightness(templ))
    if CONF_NIGHT_BRIGHTNESS in config:
        templ = await cg.templatable(config[CONF_NIGHT_BRIGHTNESS], args, float)
        cg.add(var.set_night_brightness(templ))
    return var


@automation.register_action(
    "stair_lighting.turn_off",
    StairLightingTurnOffAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(StairLightingComponent),
        }
    ),
)
async def stair_lighting_turn_off_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    return var
