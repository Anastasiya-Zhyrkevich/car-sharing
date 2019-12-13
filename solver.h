#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits.h>
#include <map>


unsigned int USE_REQUEST = 0;
unsigned int WAIT_REQUEST = 1;


class Request {
public: 
  unsigned int t;
  unsigned int dist;
  unsigned int type;

  Request(unsigned int type, unsigned int t, unsigned int dist) : type(type), t(t), dist(dist) { }
};



class Package {
public:
  unsigned int id;
  std::string name;

  Package() {}
  Package (unsigned int id, std::string name) : id(id), name(name) {}

  virtual ~Package() {}
};

class CostPackage: public Package {
public:
  unsigned int cost;
  unsigned int dist;
  unsigned int fee;  
  unsigned int t;

  CostPackage() {}

  CostPackage(unsigned int id, std::string name, unsigned int cost, unsigned int dist, unsigned int fee, unsigned int t) :
    Package(id, name), cost(cost), dist(dist), fee(fee), t(t) {} 

  std::string toString() {
    return "Buy time limited package with name=" + name; 
  }
};

class UseWaitPackage : public Package {
public:
  unsigned int use;
  unsigned int wait;

  UseWaitPackage() {}
  UseWaitPackage(unsigned int id, std::string name, unsigned int use, unsigned int wait) :
    Package(id, name), use(use), wait(wait) {} 
  
  std::string toString() {
    return "Buy per time pay package with name=" + name + 
    "use_cost=" + std::to_string(use) + "wait_cost=" + std::to_string(wait); 
  }
};

class PurchasePackage {
public:
  unsigned int id;  // Package id
  unsigned int effectiveTimeUse;
  
  PurchasePackage(unsigned int id, unsigned int effectiveTimeUse) : 
    id(id), effectiveTimeUse(effectiveTimeUse) {}
};

class Purchase {
public:
  std::vector <PurchasePackage> purchases;
  unsigned int cost; 

  Purchase() : cost(0) {}
  Purchase(std::vector <PurchasePackage> purchases, unsigned int cost): purchases(purchases), cost(cost) {}

  Purchase operator+ (const Purchase &other) {
    Purchase p;
    for (auto obj : purchases) {
      p.purchases.push_back(obj);
    }
    for (auto obj : other.purchases) {
      p.purchases.push_back(obj);
    }
    p.cost = cost + other.cost;
    return p;
  }
};


std::map<int, CostPackage> getPackages() {
  std::map<int, CostPackage> packages;

  // packages[0] = UseWaitPackage(0, "Pay-per-minute", 30, 7);
  packages[0] = CostPackage(0, "3-hours", 2400, 35, 29, 3 * 60);
  packages[1] = CostPackage(1, "6-hours", 3400, 55, 29, 6 * 60);
  packages[2] = CostPackage(2, "9-hours", 4200, 70, 29, 9 * 60);
  packages[3] = CostPackage(3, "12-hours", 4900, 80, 29, 12 * 60);
  packages[4] = CostPackage(4, "1-day", 5900, 105, 29, 24 * 60);

  return packages;
}


std::vector<Request> applyCostPackage(const std::vector<Request>& requests, 
      const CostPackage & package) {
  unsigned int dist = package.dist;
  unsigned int t = package.t;

  std::vector <Request> left;
  for (int i = 0; i < requests.size(); i++) {
    if (requests[i].dist <= dist && requests[i].t < t) {
      dist -= requests[i].dist;
      t -= requests[i].t;
      continue;
    }

    for (int j = i; j < requests.size(); j++) {
      left.push_back(requests[j]);
    }
    break;
  }

  return left;
}


Purchase getOptimalPurchases(std::vector<Request> requests, 
      const std::map<int, CostPackage> & packages, UseWaitPackage useWaitPackage) {
  Purchase optimalPurchase;
  if (requests.size() == 0) {
    return optimalPurchase;
  }

  unsigned int optimalCost = INT_MAX;

  // Try to apply CostPackage
  for (int i = 0; i < packages.size(); i++) {
    std::vector<Request> requestsLeft = applyCostPackage(requests, packages.at(i));
    if (requestsLeft.size() == requests.size()) {
      continue;
    }
    Purchase optimalTailPurchase = getOptimalPurchases(requestsLeft, packages, useWaitPackage);

    std::vector<PurchasePackage> currentVector { PurchasePackage(i, 0) };
    Purchase current = Purchase(currentVector, packages.at(i).cost);
    Purchase result = current + optimalTailPurchase;

    if (result.cost < optimalCost) {
      optimalCost = result.cost;
      optimalPurchase = result;
    }
  }

  // TODO: Try to apply UseWaitPackage 

  return optimalPurchase;
}



class Solver {
public:
  void solve(const char * in, const char * out) {
    freopen (in, "r", stdin);
    freopen (out, "w", stdout);

    int requestCnt;
    std::cin >> requestCnt;

    std::map<int, CostPackage> packages = getPackages();
    UseWaitPackage useWaitPackage = UseWaitPackage(100, "Pay-per-minute", 30, 7);

    std::vector <Request> requests;

    for (size_t i = 0; i < requestCnt; i++) {
      int requestType;
      std::cin >> requestType;

      if (requestType == USE_REQUEST) {
        unsigned int dist, t;
        std::cin >> dist >> t;

        requests.push_back(Request(requestType, t, dist));
      } else {
        unsigned int t ;
        std::cin >> t;

        requests.push_back(Request(requestType, t, 0));
      }
    }

    Purchase purchase = getOptimalPurchases(requests, packages, useWaitPackage);
    std::cout << "Purchases" << std::endl;

    for (int i = 0; i < purchase.purchases.size(); i++) {
      std::cout << packages[purchase.purchases[i].id].toString() << std::endl; 
    }
  }
};
