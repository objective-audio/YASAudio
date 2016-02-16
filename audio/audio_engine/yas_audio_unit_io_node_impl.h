//
//  yas_audio_unit_io_node_impl.h
//

#pragma once

class yas::audio::unit_io_node::impl : public super_class::impl {
    using super_class = unit_node::impl;

   public:
    impl();
    virtual ~impl();

    virtual void reset() override;

#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)
    void set_device(audio::device const &device);
    audio::device device() const;
#endif
    Float64 device_sample_rate() const;
    UInt32 output_device_channel_count() const;
    UInt32 input_device_channel_count() const;

    void set_channel_map(channel_map_t const &map, direction const dir);
    channel_map_t const &channel_map(direction const dir) const;

    virtual bus_result_t next_available_output_bus() const override;
    virtual bool is_available_output_bus(UInt32 const bus_idx) const override;

    virtual void update_connections() override;
    virtual void prepare_audio_unit() override;

   private:
    class core;
    std::unique_ptr<core> _core;
};

class yas::audio::unit_output_node::impl : public super_class::impl {
    virtual UInt32 input_bus_count() const override;
    virtual UInt32 output_bus_count() const override;
    virtual void prepare_audio_unit() override;
};

class yas::audio::unit_input_node::impl : public super_class::impl {
    using super_class = unit_io_node::impl;

   public:
    impl();
    virtual ~impl();

    virtual UInt32 input_bus_count() const override;
    virtual UInt32 output_bus_count() const override;

    virtual void update_connections() override;
    virtual void prepare_audio_unit() override;

   private:
    class core;
    std::unique_ptr<core> _core;
};
