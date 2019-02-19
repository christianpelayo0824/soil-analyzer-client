
var app = angular.module('crudApp', ['ngRoute']);
app.config(function ($routeProvider) {
    $routeProvider
        .when("/", {
            templateUrl: "main.htm",
            controller: "crudCtrl"
        })
        .when("/secondpage", {
            templateUrl: "secondpage.htm",
            controller: "crudCtrl"
        });
});


app.service('SoilAnalyzserService', function ($http) {

    var BASE_LINK = "http://192.168.254.102:8080/api";

    this.getLatestData = function () {
        return $http({
            method: 'GET',
            url: BASE_LINK + "/sensor/getLatestData"
        })
    }

    this.getRecommendedPlant = function (pHValue) {
        return $http({
            method: 'GET',
            url: BASE_LINK + "/plants/getRecommendedPlant/" + pHValue
        })
    }

    this.savePlant = function (data) {
        return $http({
            method: 'POST',
            url: BASE_LINK + "/plants/savePlants",
            data: ({
                plant: data.plant,
                min: data.min,
                max: data.max,
            })
        })
    }
})

app.controller('crudCtrl', function ($scope, $http, $interval, SoilAnalyzserService) {

    $scope.savePlant = function(plant) {
        SoilAnalyzserService.savePlant(plant)
            .then(function(response){
                console.log(response.status);
            })
    }

    $scope.getLatestData = function () {
        var elem = document.getElementById("myBar");
        var width = 10;
        var id = setInterval(frame, 10);
        function frame() {
            if (width >= 100) {
                clearInterval(id);
                $('#myModal').modal('show');
                SoilAnalyzserService.getLatestData()
                    .then(function (response) {
                        $scope.phpData = response.data
                        // console.log(response.data);
                        SoilAnalyzserService.getRecommendedPlant(response.data.phpSensor)
                            .then(function (response) {
                                console.log(response.data)
                                $scope.plants = response.data;
                            })
                    })

            } else {
                width++;
                elem.style.width = width + '%';
                elem.innerHTML = width * 1 + '%';
            }
        }

    }
});
