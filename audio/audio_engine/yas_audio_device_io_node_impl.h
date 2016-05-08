//
//  yas_audio_device_io_node_impl.h
//

#pragma once

#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)

class yas::audio::device_io_node::impl : public node::impl, public manageable_device_io_node::impl {
   public:
    impl();
    virtual ~impl();

    void prepare(device_io_node const &, audio::device const &);

    virtual UInt32 input_bus_count() const override;
    virtual UInt32 output_bus_count() const override;

    virtual void update_connections() override;

    void add_device_io() override;
    void remove_device_io() override;
    audio::device_io &device_io() const override;

    void set_device(audio::device const &device);
    audio::device device() const;

    virtual void render(pcm_buffer &buffer, UInt32 const bus_idx, time const &when) override;

   private:
    class core;
    std::unique_ptr<core> _core;

    bool _validate_connections() const;
};

#endif