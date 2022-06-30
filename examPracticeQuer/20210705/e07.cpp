Vehicle& operator=(Vehicle&& passedVehicle) noexcept {
    if (this != &passedVehicle) {
        if (this->model_name != nullptr) {
            delete model_name;
        }
        this->model_name = passedVehicle.model_name;
        delete passedVehicle.model_name;
        this->year = passedVehicle.year;
        passedVehicle.year = 0;
    }
    return *this;
}