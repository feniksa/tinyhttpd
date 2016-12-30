angular.module('directory', ['ngRoute'])
	.directive("directory", function() {
		return {
			restrict: 'E',
			templateUrl: "/assets/directory.html",
			
			controller: function($scope, $http, $routeParams) {
				this.files = {}; 
				
				console.debug($routeParams);
				
				 $http.get('/directory/' + window.location.pathname).
				    success(function(data, status, headers, config) {
				      $scope.files = data;
				    }).
				    error(function(data, status, headers, config) {
				      alert("Error fetching directory data");
				    });

			},
			controllerAs: "files"    
		};
	});

