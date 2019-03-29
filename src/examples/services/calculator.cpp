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
    
    .method("add", rttr::select_overload<double(double, double)>(&Calculator::add))(
    	rttr::parameter_names("val1", "val2"),
    	rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of scalars")
    )

	.method("add", rttr::select_overload<vector2d(vector2d, vector2d)>(&Calculator::add))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Addition of vectors")
	)

	.method("substruct", rttr::select_overload<double(double, double)>(&Calculator::substruct))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Subtruction of scalars")
	)

	.method("substruct", rttr::select_overload<vector2d(vector2d, vector2d)>(&Calculator::substruct))(
		rttr::parameter_names("val1", "val2"),
		rttr::metadata(rttr_rpc::meta_data_type::description, "Subtruction of vectors")
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
		rttr::metadata(rttr_rpc::meta_data_type::description, "Dot produuct of two vectors")
	);
}
