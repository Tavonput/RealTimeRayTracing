#pragma once

class interval {
	public:
		double min;
		double max;

		//

		bool containing(double val) {
			return min <= val && val <= max;
		}

		bool surrounding(double val) {
			return min <= val && val <= max;
		}

		static interval empty;
		static interval universe;


};