CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES7210Component),
    cv.GenerateID('microphone_id'): cv.declare_id(ES7210Microphone),
    cv.Required(CONF_I2S_AUDIO_ID): cv.use_id(i2s_audio.I2SAudioComponent),
    cv.Optional(CONF_SAMPLE_RATE, default='16000Hz'): validate_sample_rate,
    cv.Optional(CONF_BITS_PER_SAMPLE, default='16bit'): validate_bits_per_sample,
    cv.Optional('i2s_din_pin'): cv.int_,
    cv.Optional('adc_type', default='external'): cv.string_strict,
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x40))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    
    mic = cg.new_Pvariable(config['microphone_id'])
    cg.add(mic.set_parent(var))
    
    i2s = await cg.get_variable(config[CONF_I2S_AUDIO_ID])
    cg.add(var.set_i2s_audio(i2s))
    
    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))
    
    if 'i2s_din_pin' in config:
        cg.add(var.set_i2s_din_pin(config['i2s_din_pin']))
        
    cg.add(var.set_adc_type(config['adc_type']))
