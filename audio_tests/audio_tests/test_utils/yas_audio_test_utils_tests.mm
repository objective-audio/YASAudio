//
//  yas_audio_test_utils_tests.m
//

#import "yas_audio_test_utils.h"

@interface yas_audio_test_utils_tests : XCTestCase

@end

@implementation yas_audio_test_utils_tests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testIsEqualFloat64WithAccuracy {
    const Float64 val1 = 1.0;
    const Float64 accuracy = 0.1;

    Float64 val2 = 1.0;
    XCTAssertTrue(yas::test::is_equal(val1, val2, accuracy));

    val2 = 1.05;
    XCTAssertTrue(yas::test::is_equal(val1, val2, accuracy));

    val2 = 0.95;
    XCTAssertTrue(yas::test::is_equal(val1, val2, accuracy));

    val2 = 1.2;
    XCTAssertFalse(yas::test::is_equal(val1, val2, accuracy));

    val2 = 0.85;
    XCTAssertFalse(yas::test::is_equal(val1, val2, accuracy));
}

- (void)testIsEqualAudioTimeStamp {
    SMPTETime smpteTime = {
        .mSubframes = 1,
        .mSubframeDivisor = 1,
        .mCounter = 1,
        .mType = kSMPTETimeType25,
        .mFlags = 1,
        .mHours = 1,
        .mMinutes = 1,
        .mSeconds = 1,
        .mFrames = 1,
    };

    AudioTimeStamp timeStamp1 = {
        .mSampleTime = 1, .mHostTime = 1, .mRateScalar = 1, .mWordClockTime = 1, .mSMPTETime = smpteTime, .mFlags = 1,
    };

    AudioTimeStamp timeStamp2 = timeStamp1;

    XCTAssertTrue(yas::test::is_equal(&timeStamp1, &timeStamp2));

    timeStamp2 = timeStamp1;
    timeStamp2.mSampleTime = 2;

    XCTAssertFalse(yas::test::is_equal(&timeStamp1, &timeStamp2));

    timeStamp2 = timeStamp1;
    timeStamp2.mHostTime = 2;

    XCTAssertFalse(yas::test::is_equal(&timeStamp1, &timeStamp2));

    timeStamp2 = timeStamp1;
    timeStamp2.mRateScalar = 2;

    XCTAssertFalse(yas::test::is_equal(&timeStamp1, &timeStamp2));

    timeStamp2 = timeStamp1;
    timeStamp2.mWordClockTime = 2;

    XCTAssertFalse(yas::test::is_equal(&timeStamp1, &timeStamp2));

    timeStamp2 = timeStamp1;
    timeStamp2.mSMPTETime.mType = kSMPTETimeType30Drop;

    XCTAssertFalse(yas::test::is_equal(&timeStamp1, &timeStamp2));

    timeStamp2 = timeStamp1;
    timeStamp2.mFlags = 2;

    XCTAssertFalse(yas::test::is_equal(&timeStamp1, &timeStamp2));
}

- (void)testIsEqualData {
    const UInt32 size = 4;

    std::vector<UInt8> vec1(size);
    std::vector<UInt8> vec2(size);

    for (Byte i = 0; i < size; i++) {
        vec1[i] = vec2[i] = i;
    }

    XCTAssertTrue(yas::test::is_equal_data(vec1.data(), vec2.data(), size));

    vec2[0] = 4;

    XCTAssertFalse(yas::test::is_equal_data(vec1.data(), vec2.data(), size));
}

@end