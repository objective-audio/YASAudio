//
//  yas_audio_tap_node_tests.m
//  Copyright (c) 2015 Yuki Yasoshima.
//

#import <XCTest/XCTest.h>
#import "yas_audio.h"

@interface yas_audio_tap_node_tests : XCTestCase

@end

@implementation yas_audio_tap_node_tests

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)test_render_with_lambda
{
    const auto engine = yas::audio_engine::create();

    const auto output_node = yas::audio_offline_output_node::create();
    const auto to_node = yas::audio_tap_node::create();
    const auto from_node = yas::audio_tap_node::create();
    const auto format = yas::audio_format::create(48000.0, 2);

    const auto to_connection = engine->connect(to_node, output_node, format);
    const auto from_connection = engine->connect(from_node, to_node, format);

    XCTestExpectation *to_expectation = [self expectationWithDescription:@"to node"];
    XCTestExpectation *from_expectation = [self expectationWithDescription:@"from node"];
    XCTestExpectation *completion_expectation = [self expectationWithDescription:@"completion"];

    std::weak_ptr<yas::audio_tap_node> weak_to_node = to_node;
    auto to_render_func = [weak_to_node, self, to_connection, from_connection, to_expectation](
        const auto &buffer, const auto &bus_idx, const auto &when) {
        const auto &node = weak_to_node.lock();
        XCTAssertTrue(node);
        if (node) {
            XCTAssertEqual(node->output_connections_on_render().size(), 1);
            XCTAssertEqual(&*to_connection, &*node->output_connection_on_render(0));
            XCTAssertFalse(node->output_connection_on_render(1));

            XCTAssertEqual(node->input_connections_on_render().size(), 1);
            XCTAssertEqual(&*from_connection, &*node->input_connection_on_render(0));
            XCTAssertFalse(node->input_connection_on_render(1));

            node->render_source(buffer, 0, when);
        }

        [to_expectation fulfill];
    };

    to_node->set_render_function(to_render_func);

    from_node->set_render_function(
        [from_expectation](const auto &, const auto &, const auto &) { [from_expectation fulfill]; });

    XCTAssertTrue(engine->start_offline_render(
        [](const auto &, const auto &, auto &stop) { stop = true; },
        [completion_expectation](const auto cancelled) { [completion_expectation fulfill]; }));

    [self waitForExpectationsWithTimeout:0.5
                                 handler:^(NSError *error){

                                 }];

    [NSThread sleepForTimeInterval:1.0];
}

- (void)test_render_without_lambda
{
    const auto engine = yas::audio_engine::create();

    const auto output_node = yas::audio_offline_output_node::create();
    const auto to_node = yas::audio_tap_node::create();
    const auto from_node = yas::audio_tap_node::create();
    const auto format = yas::audio_format::create(48000.0, 2);

    const auto to_connection = engine->connect(to_node, output_node, format);
    const auto from_connection = engine->connect(from_node, to_node, format);

    XCTestExpectation *from_expectation = [self expectationWithDescription:@"from node"];

    from_node->set_render_function(
        [from_expectation](const auto &, const auto &, const auto &) { [from_expectation fulfill]; });

    XCTAssertTrue(engine->start_offline_render([](const auto &, const auto &, auto &stop) { stop = true; }, nullptr));

    [self waitForExpectationsWithTimeout:0.5
                                 handler:^(NSError *error){

                                 }];
}

- (void)test_bus_count
{
    auto node = yas::audio_tap_node::create();

    XCTAssertEqual(node->input_bus_count(), 1);
    XCTAssertEqual(node->output_bus_count(), 1);
}

@end