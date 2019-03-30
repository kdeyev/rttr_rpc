#include "calculator.h"

RTTR_REGISTRATION {
    rttr::registration::class_<vector2d>("vector2d")
        .constructor()(rttr::policy::ctor::as_object)
        .property("x", &vector2d::x_)(rttr::metadata(rttr_rpc::meta_data_type::description, "x coordinate"))
        .property("y", &vector2d::y_)(rttr::metadata(rttr_rpc::meta_data_type::description, "y coordinate"));

    rttr::registration::class_<Calculator>("Calculator")(
        // class meta data
        rttr::metadata(rttr_rpc::meta_data_type::thread_safe, true), 
		rttr::metadata(rttr_rpc::meta_data_type::description, "Calculator service obj"),
        rttr::metadata(rttr_rpc::meta_data_type::version, "7.0")
	)
    
    .method("add", rttr::select_overload<double(double, double)>(&Calculator::add<double>))(
    	rttr::parameter_names("val1", "val2"),
    	rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
    )
	.method("add", rttr::select_overload<float(float, float)>(&Calculator::add<float>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<int8_t(int8_t, int8_t)>(&Calculator::add<int8_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<uint8_t(uint8_t, uint8_t)>(&Calculator::add<uint8_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<int16_t(int16_t, int16_t)>(&Calculator::add<int16_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<uint16_t(uint16_t, uint16_t)>(&Calculator::add<uint16_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<int32_t(int32_t, int32_t)>(&Calculator::add<int32_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<uint32_t(uint32_t, uint32_t)>(&Calculator::add<uint32_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<int64_t(int64_t, int64_t)>(&Calculator::add<int64_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)
	.method("add", rttr::select_overload<uint64_t(uint64_t, uint64_t)>(&Calculator::add<uint64_t>))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
	)

	.method("add", rttr::select_overload<vector2d(vector2d, vector2d)>(&Calculator::add))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of vectors")
	)

	.method("multiply", rttr::select_overload<vector2d(vector2d, double)>(&Calculator::multiply))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Multiplcation vector and scalar")
	)

	.method("multiply", rttr::select_overload<vector2d(double, vector2d)>(&Calculator::multiply))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Multiplcation vector and scalar")
	)

	.method("dot", &Calculator::dot)(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Dot product of two vectors")
	);
}
