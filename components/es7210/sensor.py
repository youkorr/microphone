import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, microphone
from esphome.const import CONF_ID, CONF_BITS_PER_SAMPLE, CONF_SAMPLE_RATE

DEPENDENCIES = ['i2c']
MULTI_CONF = True

es7210_ns = cg.esphome_ns.namespace('es7210')
ES7210Component = es7210_ns.class_('ES7210Component', cg.Component, i2c.I2CDevice)
ES7210Microphone = es7210_ns.class_('ES7210Microphone', microphone.Microphone)

def validate_bits_per_sample(value):
    if isinstance(value, str):
        if value.endswith('bit'):
            value = int(value[:-3])
    if value not in [16, 24, 32]:
        raise cv.Invalid("bits_per_sample must be 16, 24 or 32")
    return value

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES7210Component),
    cv.Required(CONF_ID): cv.use_id(ES7210Microphone),
    cv.Optional(CONF_SAMPLE_RATE, default=16000): cv.int_range(min=8000, max=48000),
    cv.Optional(CONF_BITS_PER_SAMPLE, default='16bit'): validate_bits_per_sample,
    cv.Optional('i2s_port', default=0): cv.one_of(0, 1),
}).extend(i2c.i2c_device_schema(0x40))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))
    cg.add(var.set_i2s_port(config['i2s_port']))

    mic = cg.new_Pvariable(config[CONF_ID])
    await microphone.register_microphone(mic, config)
    cg.add(mic.set_es7210(var))
