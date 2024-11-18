import esphome.codegen as cg
from esphome.components import light
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESSABLE_LIGHT_ID, CONF_ID

CONF_START = "start"
CONF_END = "end"

stair_lighting_ns = cg.esphome_ns.namespace("stair_lighting")

StairLightingComponent = stair_lighting_ns.class_(
    "StairLightingComponent", cg.PollingComponent
)
StairLightingStep = stair_lighting_ns.class_("StairLightingStep")
StairLightingEffect = stair_lighting_ns.class_("StairlightingEffect")


STEP_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(StairLightingStep),
        cv.Required(CONF_ADDRESSABLE_LIGHT_ID): cv.use_id(light.AddressableLightState),
        cv.Required(CONF_START): cv.positive_int,
        cv.Required(CONF_END): cv.positive_int,
    }
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(StairLightingComponent),
        cv.Required("steps"): cv.All(cv.ensure_list(STEP_SCHEMA), cv.Length(min=1)),
        cv.Optional("night_brightness", default="10%"): cv.percentage,
        cv.Optional("effect_brightness", default="100%"): cv.percentage,
    }
).extend(cv.COMPONENT_SCHEMA)


async def step_to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_light = await cg.get_variable(config[CONF_ADDRESSABLE_LIGHT_ID])
    cg.add(var.set_light(wrapped_light))
    cg.add(var.set_start(config[CONF_START]))
    cg.add(var.set_end(config[CONF_END]))
    cg.add(var.init())
    return var


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    steps = []
    for step_config in config.get("steps", []):
        steps.append(await step_to_code(step_config))
    cg.add(var.add_steps(steps))
    cg.add(var.set_night_brightness(config["night_brightness"]))
    cg.add(var.set_effect_brightness(config["effect_brightness"]))

    await cg.register_component(var, config)
