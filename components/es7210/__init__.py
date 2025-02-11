import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, i2s_audio, microphone
from esphome.const import CONF_ID, CONF_BITS_PER_SAMPLE, CONF_SAMPLE_RATE

DEPENDENCIES = ['i2c', 'i2s_audio']
AUTO_LOAD = ['microphone']
CODEOWNERS = ['@youkorr']

CONF_I2S_AUDIO = "i2s_audio_id"

es7210_ns = cg.esphome_ns.namespace("es7210")
ES7210Microphone = es7210_ns.class_("ES7210Microphone", microphone.Microphone, cg.Component, i2c.I2CDevice)

def validate_gpio_pin(value):
    if isinstance(value, str) and value.startswith("GPIO"):
        return int(value[4:])
    return cv.int_(value)

# Définissez d'abord votre schéma de configuration de base
BASE_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES7210Microphone),
    cv.Required(CONF_I2S_AUDIO): cv.use_id(i2s_audio.I2SAudioComponent),
    cv.Optional(CONF_SAMPLE_RATE, default="16000Hz"): cv.frequency,
    cv.Optional(CONF_BITS_PER_SAMPLE, default="16bit"): cv.one_of(16, 24, 32, int=True),
    cv.Optional("i2c_id"): cv.use_id(i2c.I2CBus),
    cv.Optional("i2s_din_pin"): validate_gpio_pin,
    cv.Optional("adc_type", default="external"): cv.string_strict,
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x40))

# Enregistrez le composant en tant que plateforme de microphone
CONFIG_SCHEMA = microphone.MICROPHONE_PLATFORM_SCHEMA.extend(BASE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await microphone.register_microphone(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_I2S_AUDIO in config:
        i2s = await cg.get_variable(config[CONF_I2S_AUDIO])
        cg.add(var.set_i2s_audio(i2s))

    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))

    if "i2s_din_pin" in config:
        cg.add(var.set_i2s_din_pin(config["i2s_din_pin"]))

    if "adc_type" in config:
        cg.add(var.set_adc_type(config["adc_type"]))
